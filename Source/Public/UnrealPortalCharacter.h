#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealPortalCharacter.generated.h"



UCLASS(config=Game)
class AUnrealPortalCharacter : public ACharacter
{
    GENERATED_UCLASS_BODY()
	
public:
   UFUNCTION(BlueprintCallable)
   UCameraComponent* GetPlayerCamera() const;

   UFUNCTION()
   AUnrealPortalPlayerController* GetUPC() const;

   virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

protected:
   UPROPERTY(VisibleAnywhere, Category = "Mesh")
   UCameraComponent* PlayerCamera;

   UPROPERTY()
   AUnrealPortalPlayerController* UPC;
}