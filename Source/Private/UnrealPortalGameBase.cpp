#include "UnrealPortalGameModeBase.h"
#include "UnrealPortalCharacter.h"
#include "UnrealPortalPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AUnrealPortalGameModeBase::AUnrealPortalGameModeBase()
	: Super()
{
    PlayerControllerClass = AUnrealPortalPlayerController.StaticClass();

}