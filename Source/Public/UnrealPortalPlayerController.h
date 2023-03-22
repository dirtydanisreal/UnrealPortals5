#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UnrealPortalPlayerController.generated.h"


UCLASS()
class UNREALPORTAL_API AUnrealPortalPlayerController : public APlayerController
{
    GENERATED_BODY()

    public:

    FMatrix GetCameraProjectionMatrix() const;

    AUnrealPortalManager* GetPortalManager() const { return PortalManager; }

    protected:

    virtual void BeginPlay() override;

    public:

    AUnrealPortalManager* PortalManager;
}