#include "SpartaGameMode.h"
#include "SpartaPawn.h"
#include "SpartaPlayerController.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ASpartaPawn::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
}
