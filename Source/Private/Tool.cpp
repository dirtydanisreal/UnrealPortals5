#include "Tool.h"
#include "UnrealPortal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "UnrealPortalPlayerController.h"
#include "UnrealPortalManager.h"
#include <Math/UnrealMathUtility.h> 
#include <Editor/DetailCustomizations/Private/SceneCaptureDetails.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h> 
#include <Camera/CameraComponent.h>

bool UTool::IsPointInsideBox( FVector Point, UBoxComponent* Box )
{
    if( Box != nullptr )
    {
        // From :
        // https://stackoverflow.com/questions/52673935/check-if-3d-point-inside-a-box/52674010

        FVector Center      = Box->GetComponentLocation();
        FVector Half        = Box->GetScaledBoxExtent();
        FVector DirectionX  = Box->GetForwardVector();
        FVector DirectionY  = Box->GetRightVector();
        FVector DirectionZ  = Box->GetUpVector();

        FVector Direction   = Point - Center;

        bool IsInside = FMath::Abs( FVector::DotProduct( Direction, DirectionX ) ) <= Half.X &&
                        FMath::Abs( FVector::DotProduct( Direction, DirectionY ) ) <= Half.Y &&
                        FMath::Abs( FVector::DotProduct( Direction, DirectionZ ) ) <= Half.Z;

        return IsInside;
    }
    else
    {
        return false;
    }
}

FVector UTool::ConvertLocationToActorSpace( FVector Location, AActor* Reference, AActor* Target )
{
    if( Reference == nullptr || Target == nullptr )
    {
        return FVector::ZeroVector;
    }

    FVector Direction       = Location - Reference->GetActorLocation();
    FVector TargetLocation  = Target->GetActorLocation();

    FVector Dots;
    Dots.X  = FVector::DotProduct( Direction, Reference->GetActorForwardVector() );
    Dots.Y  = FVector::DotProduct( Direction, Reference->GetActorRightVector() );
    Dots.Z  = FVector::DotProduct( Direction, Reference->GetActorUpVector() );

    FVector NewDirection    = Dots.X * Target->GetActorForwardVector()
                            + Dots.Y * Target->GetActorRightVector()
                            + Dots.Z * Target->GetActorUpVector();

    return TargetLocation + NewDirection;
}

FRotator UTool::ConvertRotationToActorSpace( FRotator Rotation, AActor* Reference, AActor* Target )
{
    if( Reference == nullptr || Target == nullptr )
    {
        return FRotator::ZeroRotator;
    }

    FTransform SourceTransform  = Reference->GetActorTransform();
    FTransform TargetTransform  = Target->GetActorTransform();
    FQuat QuatRotation          = FQuat( Rotation );

    FQuat LocalQuat             = SourceTransform.GetRotation().Inverse() * QuatRotation;
    FQuat NewWorldQuat          = TargetTransform.GetRotation() * LocalQuat;

    return NewWorldQuat.Rotator();
}

AUnrealPortalManager* UTool::GetPortalManager( AActor* Context )
{
    AUnrealPortalManager* Manager = nullptr;

    //Retrieve the World from the Context actor
    if( Context != nullptr && Context->GetWorld() != nullptr )
    {
        //Find PlayerController
        AUnrealPlayerController* UPC = Cast<AUnrealPlayerController>( Context->GetWorld()->GetFirstPlayerController() );

        //Retrieve the Portal Manager
        if( UPC != nullptr && UPC->GetPortalManager() != nullptr )
        {
            Manager = UPC->GetPortalManager();
        }
    }

    return Manager;
}

