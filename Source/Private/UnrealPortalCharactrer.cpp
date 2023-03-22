#include "UnrealPortalCharacter.h"
#include "UnrealPortalPlayerController.h"
#include "Camera/CameraComponent.h" 
#include "Components/CapsuleComponent.h" 

AUnrealPortalCharacter::AUnrealPortalCharacter(const FObjectInitializer& ObjectInitializer) :
   Super(ObjectInitializer)
{
   PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
   PlayerCamera->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
   PlayerCamera->Mobility = EComponentMobility::Movable;
   PlayerCamera->bUsePawnControlRotation = true;

   GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

UCameraComponent* AUnrealPortalCharacter::GetPlayerCamera() const
{
   return PlayerCamera;
}

AUnrealPortalPlayerController* AUnrealPortalCharacter()::GetUPC() const
{
    return CastChecked<AUnrealPortalPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

void AUnrealPortalCharacter::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
    Super::TickActor( DeltaTime, TickType, ThisTickFunction );

    if( UGameplayStatics::GetPlayerController(GetWorld(), 0) != nullptr )
    {
        AUnrealPortalPlayerController* UPC = GetUPC();
        UPC->PortalManager->Update( DeltaTime );
    }
}