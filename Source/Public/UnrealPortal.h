#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealPortal.generated.h"

class AUnrealPortalCharacter;

UCLASS()
class UNREALPORTALS_API AUnrealPortal : public AActor
{
    GENERATED_UCLASS_BODY()

    public:

        virtual void Tick(float DeltaTime) override;

        UFUNCTION(BlueprintPure, Category="Portal")
        bool IsActive();

        UFUNCTION(BlueprintCallable, Category="Portal")
        void SetActive( bool NewActive );

        //Render target to use to display the portal
        UFUNCTION(BlueprintCallable, BlueprintCallable, Category="Portal")
        void ClearRTT();

        UFUNCTION(BlueprintCallable, BlueprintCallable, Category="Portal")
        void SetRTT( UTexture* RenderTexture );

        UFUNCTION(BlueprintCallable, Category="Portal")
        void ForceTick();

        //Target of where the portal is looking
        UFUNCTION(BlueprintPure, Category="Portal")
        AActor* GetTarget();

        UFUNCTION(BlueprintCallable, Category="Portal")
        void SetTarget( AActor* NewTarget );

        //Helpers
        UFUNCTION(BlueprintCallable, Category="Portal")
        bool IsPointInFrontOfPortal( FVector Point, FVector PortalLocation, FVector PortalNormal );

        UFUNCTION(BlueprintCallable, Category="Portal")
        bool IsPointCrossingPortal( FVector Point, FVector PortalLocation, FVector PortalNormal );

        UFUNCTION(BlueprintCallable, Category="Portal")
        void TeleportActor( AActor* ActorToTeleport );

    protected:
        UPROPERTY(BlueprintReadOnly)
        USceneComponent* PortalRootComponent;

        virtual void BeginPlay() override;

    private:
        bool bIsActive;

        AActor* Target;

        //Used for Tracking movement of a point
        FVector LastPosition;
        bool    LastInFront;

    public:

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	    UStaticMeshComponent* MeshComponent;

	    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	    UBoxComponent* PortalArea;

	    UPROPERTY(EditAnywhere, Category = Capture)
	    UTextureRenderTarget2D* RenderTarget;

	    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	    UMaterial* Material;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
        AActor* PortalTarget;
};