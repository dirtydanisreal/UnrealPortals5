#include "UnrealPortalManager.h"
#include <Components/ActorComponent.h>
#include <Engine/TextureRenderTarget2D.h>
#include "UnrealPortalPlayerController.h"
#include "UnrealPortal.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include "UnrealCharacter.h"

AUnrealPortalManager::AUnrealPortalManager(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    PortalTexture = nullptr;
    UpdateDelay = 1.1f;

    PreviousScreenSizeX = 0;
    PreviousScreenSizeY = 0;
}

void AUnrealPortalManager::SetControllerOwner( AUnrealPlayerController* NewOwner )
{
    ControllerOwner = NewOwner;

    
}

void AExedrePortalManager::Init()
{
    //------------------------------------------------
    //Create Camera
    //------------------------------------------------
    SceneCapture = NewObject<USceneCaptureComponent2D>(this, USceneCaptureComponent2D::StaticClass(), *FString("PortalSceneCapture"));

    SceneCapture->AttachToComponent( GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale );
    SceneCapture->RegisterComponent();

    SceneCapture->bCaptureEveryFrame            = false;
    SceneCapture->bCaptureOnMovement            = false;
    SceneCapture->LODDistanceFactor             = 3; //Force bigger LODs for faster computations
    SceneCapture->TextureTarget                 = nullptr;
    SceneCapture->bEnableClipPlane              = true;
    SceneCapture->bUseCustomProjectionMatrix    = true;
    SceneCapture->CaptureSource                 = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;

    //Setup Post-Process of SceneCapture (optimization : disable Motion Blur, etc)
    FPostProcessSettings CaptureSettings;

    CaptureSettings.bOverride_AmbientOcclusionQuality       = true;
    CaptureSettings.bOverride_MotionBlurAmount              = true;
    CaptureSettings.bOverride_SceneFringeIntensity          = true;
    CaptureSettings.bOverride_FilmGrainIntensity                = true;
    CaptureSettings.bOverride_ScreenSpaceReflectionQuality  = true;

    CaptureSettings.AmbientOcclusionQuality         = 0.0f; //0=lowest quality..100=maximum quality
    CaptureSettings.MotionBlurAmount                = 0.0f; //0 = disabled
    CaptureSettings.SceneFringeIntensity            = 0.0f; //0 = disabled
    CaptureSettings.FilmGrainIntensity                  = 0.0f; //0 = disabled
    CaptureSettings.ScreenSpaceReflectionQuality    = 0.0f; //0 = disabled

    

    SceneCapture->PostProcessSettings = CaptureSettings;

    //------------------------------------------------
    //Create RTT Buffer
    //------------------------------------------------
    GeneratePortalTexture();
}

void AUnrealPortalManager::GeneratePortalTexture()
{
    int32 CurrentSizeX = 1920;
    int32 CurrentSizeY = 1080;

    if( ControllerOwner != nullptr )
    {
        ControllerOwner->GetViewportSize(CurrentSizeX, CurrentSizeY);
    }

    // Use a smaller size than the current 
    // screen to reduce the performance impact
    CurrentSizeX = FMath::Clamp( int(CurrentSizeX / 1.7), 128, 1920); //1920 / 1.5 = 1280
    CurrentSizeY = FMath::Clamp( int(CurrentSizeY / 1.7), 128, 1080);

    if( CurrentSizeX == PreviousScreenSizeX
    &&  CurrentSizeY == PreviousScreenSizeY )
    {
        return;
    }

    PreviousScreenSizeX = CurrentSizeX;
    PreviousScreenSizeY = CurrentSizeY;


    // Create the RenderTarget if it does not exist
    if( PortalTexture == nullptr )
    {
        // Create new RTT
        PortalTexture = NewObject<UTextureRenderTarget2D>(
            this,
            UTextureRenderTarget2D::StaticClass(),
            *FString("PortalRenderTarget")
        );
        check( PortalTexture );

        PortalTexture->RenderTargetFormat   = ETextureRenderTargetFormat::RTF_RGBA16f;
        PortalTexture->Filter               = TextureFilter::TF_Bilinear;
        PortalTexture->SizeX                = CurrentSizeX;
        PortalTexture->SizeY                = CurrentSizeY;
        PortalTexture->ClearColor           = FLinearColor::Black;
        PortalTexture->TargetGamma          = 2.2f;
        PortalTexture->bNeedsTwoCopies      = false;
        PortalTexture->AddressX             = TextureAddress::TA_Clamp;
        PortalTexture->AddressY             = TextureAddress::TA_Clamp;

        // Not needed since the texture is displayed on screen directly
        // in some engine versions this can even lead to crashes (notably 4.24/4.25)
        PortalTexture->bAutoGenerateMips    = false;

        // This force the engine to create the render target 
        // with the parameters we defined just above
        PortalTexture->UpdateResource();
    }
    // Resize the RenderTarget if it already exists
    else
    {
        PortalTexture-> ResizeTarget( CurrentSizeX, CurrentSizeY );
    }
}

void AUnrealPortalManager::Update( float DeltaTime )
{
    //-----------------------------------
    // Generate Portal texture ?
    //-----------------------------------
    UpdateDelay += DeltaTime;

    if( UpdateDelay > 1.0f )
    {
        UpdateDelay = 0.0f;
        GeneratePortalTexture();
    }

    //-----------------------------------
    // Find portals in the level and update them
    //-----------------------------------
    AUnrealPortal* Portal = UpdatePortalsInWorld();

    if( Portal != nullptr )
    {
        UpdateCapture( Portal );
    }
}

AUnrealPortal* AUnrealPortalManager::UpdatePortalsInWorld()
{
    if( ControllerOwner == nullptr )
    {
        return nullptr;
    }

    AUnrealPortalCharacter* Character = Cast<AUnrealPortalCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));

    //-----------------------------------
    // Update Portal actors in the world (and active one if nearby)
    //-----------------------------------
    AUnrealPortal* ActivePortal = nullptr;
    FVector PlayerLocation      = Character->GetActorLocation();
    float Distance              = 4096.0f;

    for( TActorIterator<AUnrealPortal>ActorItr( GetWorld() ); ActorItr; ++ActorItr )
    {
        AUnrealPortal* Portal   = *ActorItr;
        FVector PortalLocation  = Portal->GetActorLocation();
        FVector PortalNormal    = -1 * Portal->GetActorForwardVector();

        // Reset Portal
        Portal->ClearRTT();
        Portal->SetActive( false );

        // Find the closest Portal when the player is Standing in front of
        float NewDistance = FMath::Abs( FVector::Dist( PlayerLocation, PortalLocation ) );

        if( NewDistance < Distance )
        {
            Distance        = NewDistance;
            ActivePortal    = Portal;
        }
    }

    return ActivePortal;
}

void AUnrealPortalManager::UpdateCapture( AUnrealPortal* Portal )
{
    if( ControllerOwner == nullptr )
    {
        return;
    }

    AUnrealPortalCharacter* Character = Cast<AUnrealPortalCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));


    //-----------------------------------
    // Update SceneCapture (discard if there is no active portal)
    //-----------------------------------
    if(SceneCapture     != nullptr
    && PortalTexture    != nullptr
    && Portal   != nullptr
    && Character        != nullptr )
    {

        UCameraComponent* PlayerCamera = Character->GetPlayerCamera();
        AActor* Target  = Portal->GetTarget();

        //Place the SceneCapture to the Target
        if( Target != nullptr )
        {
            //-------------------------------
            // Compute new location in the space of the target actor
            // (which may not be aligned to world)
            //-------------------------------
            FVector NewLocation     = UTool::ConvertLocationToActorSpace(   PlayerCamera->GetComponentLocation(),
                                                                            Portal,
                                                                            Target );

            SceneCapture->SetWorldLocation( NewLocation );

            //-------------------------------
            //Compute new Rotation in the space of the
            //Target location
            //-------------------------------
            FTransform CameraTransform  = PlayerCamera->GetComponentTransform();
            FTransform SourceTransform  = Portal->GetActorTransform();
            FTransform TargetTransform  = Target->GetActorTransform();

            FQuat LocalQuat             = SourceTransform.GetRotation().Inverse() * CameraTransform.GetRotation();
            FQuat NewWorldQuat          = TargetTransform.GetRotation() * LocalQuat;

            //Update SceneCapture rotation
            SceneCapture->SetWorldRotation( NewWorldQuat );

            //-------------------------------
            //Clip Plane : to ignore objects between the
            //SceneCapture and the Target of the portal
            //-------------------------------
            SceneCapture->ClipPlaneNormal   = Target->GetActorForwardVector();
            SceneCapture->ClipPlaneBase     = Target->GetActorLocation()
                                            + (SceneCapture->ClipPlaneNormal * -1.5f); //Offset to avoid visible pixel border
        }

        // Switch on the valid Portal
        Portal->SetActive( true );

        // Assign the Render Target
        Portal->SetRTT( PortalTexture );
        SceneCapture->TextureTarget = PortalTexture;

        // Get the Projection Matrix
        SceneCapture->CustomProjectionMatrix = ControllerOwner->GetCameraProjectionMatrix();

        // Say Cheeeeese !
        SceneCapture->CaptureScene();
    }
}

void AUnrealPortalManager::RequestTeleportByPortal( AUnrealPortal* Portal, AActor* TargetToTeleport )
{
    if( Portal != nullptr && TargetToTeleport != nullptr )
    {
        if( ControllerOwner != nullptr )
        {
            SceneCapture->bCameraCutThisFrame = 1;
        }
        Portal->TeleportActor( TargetToTeleport );

        //-----------------------------------
        //Force update
        //-----------------------------------
        AUnrealPortal* FuturePortal = UpdatePortalsInWorld();

        if( FuturePortal != nullptr )
        {
            FuturePortal->ForceTick(); //Force update before the player render its view since he just teleported
            UpdateCapture( FuturePortal );
        }
    }
}