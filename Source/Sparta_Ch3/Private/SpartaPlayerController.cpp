#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//IMC활성화 코드
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())//현재 PlayerController에 연결된 Local Player 객체를 가져온다.
	{
		//LocalPlayer에서 EnhancedInputLocalPlayerSubsystem을 획득?
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				//내가 만든 IMC가 있으면 활성화
				//우선순위는 0이 가장 높은 순서
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
