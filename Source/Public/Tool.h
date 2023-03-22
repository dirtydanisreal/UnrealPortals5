#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MathLibrary.generated.h"

struct FHitResult;
struct FCollisionQueryParams;

UCLASS()
class UNREALPORTAL_API UTool : public UObject
{
    GENERATED_BODY()

    public:

    static FVector ConvertLocationToActorSpace( FVector Location, AActor* Reference, AActor* Target );

    static FRotator ConvertRotationToActorSpace( FRotator Rotation, AActor* Reference, AActor* Target );

    UFUNCTION(BlueprintCallable,Category="Portal|Tools")
    static bool IsPointInsideBox( FVector Point, UBoxComponent* Box );

    UFUNCTION(BlueprintPure,Category="Portal|Tools")
    static AUnrealPortalManager* GetPortalManager( AActor* Context );

    static bool IsVertexHidden(FHitResult& OUT_hit_result, USceneComponent* camera, const FVector& vertex, FCollisionQueryParams& params);
}