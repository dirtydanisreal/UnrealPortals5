#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealPortalManager.generated.h"

class AUnrealPortalPlayerController;
class AUnrealPortal;
class UTextureRenderTarget2D;


UCLASS()
class UNREALPORTALS_API AUnrealPortalManager : public AActor
{
    GENERATED_UCLASS_BODY()

    public: 

    AUnrealPortalManager();

     // Called by a Portal actor when wanting to teleport something
        UFUNCTION(BlueprintCallable, Category="Portal")
        void RequestTeleportByPortal( AUnrealPortal* Portal, AActor* TargetToTeleport );

        // Save a reference to the PlayerControler
        void SetControllerOwner( AUnrealPortalPlayerController* NewOwner );

        // Various setup that happens during spawn
        void Init();

        // Manual Tick
        void Update( float DeltaTime );

        // Find all the portals in world and update them
        // returns the most valid/usable one for the Player
        AUnrealPortal* UpdatePortalsInWorld();

        // Update SceneCapture
        void UpdateCapture( AUnrealPortal* Portal );

        private:
        //Function to create the Portal render target
        void GeneratePortalTexture();

        UPROPERTY()
        USceneCaptureComponent2D* SceneCapture;

        UPROPERTY(transient)
        UTextureRenderTarget2D* PortalTexture;

        UPROPERTY()
        AUnrealPlayerController* ControllerOwner;

        int32 PreviousScreenSizeX;
        int32 PreviousScreenSizeY;

        float UpdateDelay;
};