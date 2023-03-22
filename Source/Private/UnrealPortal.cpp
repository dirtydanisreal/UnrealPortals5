#include "UnrealPortal.h"
#include <Components/ActorComponent.h>
#include <Engine/TextureRenderTarget2D.h>
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Tool.h"
#include "UnrealCharacter.h"
#include "UnrealPortalManager.h"
#include "UnrealPortalPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"



AUnrealPortal::AUnrealPortal(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick   = true;
    bIsActive                       = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent->Mobility = EComponentMobility::Static;

    PortalRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRootComponent"));
    PortalRootComponent->SetupAttachment( GetRootComponent() );
    PortalRootComponent->SetRelativeLocation( FVector(0.0f, 0.0f, 0.0f) );
    PortalRootComponent->SetRelativeRotation( FRotator(0.0f, 0.0f, 0.0f) );
    PortalRootComponent->Mobility = EComponentMobility::Movable;

    PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
    PortalMesh->SetupAttatchment(PortalRootComponent);

    PortalArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Portal_Area"));
    PortalArea->SetupAttatchment(PortalRootComponent);
}

bool AUnrealPortal::IsActive()
{
    return bIsActive;
}

void AUnrealPortal::SetActive( bool NewActive )
{
    bIsActive = NewActive;
}

void AUnrealPortal::ClearRTT()
{
    MaterialInstance->SetTextureParameterValue(FName("PortalTexture"),nullptr);
}

void AUnrealPortal::SetRTT( UTexture* RenderTexture )
{
    if(IsValid != false)
    {
        MaterialInstance->SetTextureParameterValue(FName("PortalTexture"),RenderTexture);
    }
}

void AUnrealPortal::ForceTick()
{
    AUnrealPortalCharacter* Character = Cast<AUnrealPortalCharacter>(UGameplayStatics::GetPlayerCharacter(GetWold(),0);)
    UCameraComponent* PlayerCamera = Character->GetPlayerCamera();
    if(UTool::IsPointInsideBox(PlayerCamera->GetComponentLocaltion(),PortalArea) != false)
    {
        MaterialInstance->SetScalarParameterValue(FName("ScaleVertex"),1.0);
    }
    else
    {
        MaterialInstance->SetScalarParameterValue(FName("ScaleVertex"),0.0);
    }
}

void AUnrealPortal::BeginPlay()
{
    Super::BeginPlay();

    SetTarget(PortalTarget);
    MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
    PortalMesh->SetMaterial(0,MaterialInstance);
    MaterialInstance->SetTextureParameterValue(FName("PortalTexture"),nullptr);
}

AActor* AUnrealPortal::GetTarget()
{
    return Target;
}

void AUnrealPortal::SetTarget( AActor* NewTarget )
{
    Target = NewTarget;
}

bool AUnrealPortal::IsPointInFrontOfPortal( FVector Point, FVector PortalLocation, FVector PortalNormal )
{
    FPlane PortalPlane  = FPlane( PortalLocation, PortalNormal );
    float PortalDot     = PortalPlane.PlaneDot( Point );

    //If < 0 means we are behind the Plane
    //See : http://api.unrealengine.com/INT/API/Runtime/Core/Math/FPlane/PlaneDot/index.html
    return ( PortalDot >= 0 );
}

bool AUnrealPortal::IsPointCrossingPortal( FVector Point, FVector PortalLocation, FVector PortalNormal )
{
    FVector IntersectionPoint;
    FPlane PortalPlane  = FPlane( PortalLocation, PortalNormal );
    float PortalDot     = PortalPlane.PlaneDot( Point );
    bool IsCrossing     = false;
    bool IsInFront      = PortalDot >= 0;

    bool IsIntersect    = FMath::SegmentPlaneIntersection(  LastPosition,
                                                            Point,
                                                            PortalPlane,
                                                            IntersectionPoint );

    // Did we intersect the portal since last Location ?
    // If yes, check the direction : crossing forward means we were in front and now at the back
    // If we crossed backward, ignore it (similar to Prey 2006)
    if( IsIntersect && !IsInFront && LastInFront )
    {
        IsCrossing  = true;
    }

    // Store values for next check
    LastInFront     = IsInFront;
    LastPosition    = Point;

    return IsCrossing;
}

void AUnrealPortal::TeleportActor( AActor* ActorToTeleport )
{
    if( ActorToTeleport == nullptr || Target == nullptr )
    {
        return;
    }

    //-------------------------------
    //Retrieve and save Player Velocity
    //(from the Movement Component)
    //-------------------------------
    FVector SavedVelocity   = FVector::ZeroVector;
    AUnrealPortalCharacter* UC    = nullptr;

    if( ActorToTeleport->IsA( AUnrealPortalCharacter::StaticClass() ) )
    {
        UC = Cast<AUnrealPortalCharacter>( ActorToTeleport );

        SavedVelocity = EC->GetCharacterMovement()->Velocity;
    }

    //-------------------------------
    //Compute and apply new location
    //-------------------------------
    FHitResult HitResult;
    FVector NewLocation = UTool::ConvertLocationToActorSpace(   ActorToTeleport->GetActorLocation(),
                                                                this,
                                                                Target );

    ActorToTeleport->SetActorLocation(  NewLocation,
                                        false,
                                        &HitResult,
                                        ETeleportType::TeleportPhysics );

    //-------------------------------
    //Compute and apply new rotation
    //-------------------------------
    FRotator NewRotation = UTool::ConvertRotationToActorSpace(  ActorToTeleport->GetActorRotation(),
                                                                this,
                                                                Target );

    //Apply new rotation
    ActorToTeleport->SetActorRotation( NewRotation );

    //-------------------------------
    //If we are teleporting a character we need to
    //update its controller as well and reapply its velocity
    //-------------------------------
    if( ActorToTeleport->IsA( AExedreCharacter::StaticClass() ) )
    {
        //Update Controller
        AUnrealPortalPlayerController* UPC = UC->GetUPC();

        if( UPC != nullptr )
        {
            NewRotation = UTool::ConvertRotationToActorSpace(   UPC->GetControlRotation(),
                                                                this,
                                                                Target );

            UPC->SetControlRotation( NewRotation );
        }

        //Reapply Velocity (Need to reorient direction into local space of Portal)
        {
            FVector Dots;
            Dots.X  = FVector::DotProduct( SavedVelocity, GetActorForwardVector() );
            Dots.Y  = FVector::DotProduct( SavedVelocity, GetActorRightVector() );
            Dots.Z  = FVector::DotProduct( SavedVelocity, GetActorUpVector() );

            FVector NewVelocity     = Dots.X * Target->GetActorForwardVector()
                                    + Dots.Y * Target->GetActorRightVector()
                                    + Dots.Z * Target->GetActorUpVector();

            UC->GetCharacterMovement()->Velocity = NewVelocity;
        }
    }

    //Cleanup Teleport
    LastPosition = NewLocation;
}

void AUnrealPortal::Tick(float DeltaTime)
{
    MaterialInstance->SetScalarParameterValue(FName("ScaleVertex",0.0));
    if(IsActive() != false)
    {
        AUnrealPortalCharacter* Character = Cast<AUnrealPortalCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
        UCameraComponent* PlayerCamera = Character->GetPlayerCamera();
        FVector PortalLocation = PortalRootComponent->GetComponentLocation();
        FVector PortalNormal = PortalRootComponent->GetForwardVector() * -1;
        bool isCrossing = IsPointCrossingPortal(PlayerCamera,PortalLocation,PortalNormal);

        if(isCrossing != false)
        {
            
        }
    }
}