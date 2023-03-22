#include "UnrealPortalGameModeBase.h"
#include "UnrealPortalCharacter.h"
#include "UnrealPortalPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AUnrealPortalGameMode::AUnrealPortalGameMode()
	: Super()
{
    PlayerControllerClass = AUnrealPortalPlayerController.StaticClass();

}