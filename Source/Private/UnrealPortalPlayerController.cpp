#include "UnrealPortalPlayerController.h"
#include "UnrealPortalManager.h"


FMatrix AUnrealPortalPlayerController::GetCameraProjectionMatrix() const
{
    FMatrix ProjectionMatrix;

	if (GetLocalPlayer() != nullptr)
	{
		FSceneViewProjectionData PlayerProjectionData;
		GetLocalPlayer()->GetProjectionData(
			GetLocalPlayer()->ViewportClient->Viewport,
			PlayerProjectionData,
			INDEX_NONE
		);

		ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
	}

	return ProjectionMatrix;
}

void AUnrealPortalPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FActorSpawnParameters SpawnParams;

    PortalManager = nullptr;
    PortalManager = GetWorld()->SpawnActor<AUnrealPortalManager>(   AExedrePortalManager::StaticClass(),
                                                                    FVector::ZeroVector,
                                                                    FRotator::ZeroRotator,
                                                                    SpawnParams);
    PortalManager->AttachToActor( this, FAttachmentTransformRules::SnapToTargetIncludingScale);
    PortalManager->SetControllerOwner( this );
    PortalManager->Init();
}