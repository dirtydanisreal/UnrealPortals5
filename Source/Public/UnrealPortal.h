#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealPortal.generated.h"

class AUnrealPortalCharacter;

USTRUCT(BlueprintType)
struct FTextureToRender {
   GENERATED_BODY()

public:
   FTextureToRender() : texture(nullptr), is_mirror(false), weight(0.f) {};
   FTextureToRender(FTextureToRender&& other) noexcept;
   FTextureToRender(const FTextureToRender& other) noexcept;
   FTextureToRender& operator=(FTextureToRender&& other) noexcept;
   FTextureToRender& operator=(const FTextureToRender& other) noexcept;

   UPROPERTY(BlueprintReadOnly)
   const UTexture* texture;

   UPROPERTY(BlueprintReadOnly)
   bool is_mirror;

   UPROPERTY(BlueprintReadOnly)
   float weight;
};

UCLASS()
class UNREALPORTALS_API AUnrealPortal : public AActor
{
    GENERATED_UCLASS_BODY()

    public:

        FPlane GetPortalPlane() const { return FPlane(GetActorLocation(), GetActorForwardVector()); }

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

        void LoadMeshVertices() const;

        const TArray<FVector>* GetMeshVertices() const;

        FVector GetMiddlePoint() const { return m_middle_point; }

    protected:
        UPROPERTY(BlueprintReadOnly)
        USceneComponent* PortalRootComponent;

        virtual void BeginPlay() override;

        mutable TArray<FVector> m_vertices;

        mutable FVector m_middle_point;

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

        UPROPERTY(Instanced, VisibleInstanceOnly, BlueprintReadWrite, Category = Portal, meta = (ExposeOnSpawn="true"))
        AActor* PortalTarget;
};