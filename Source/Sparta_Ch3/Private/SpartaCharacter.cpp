#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//속도를 처리하기 위해 Character의 maxspeed를 가져오기 위해서 포함하는 헤더

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	bIsSprinting = false;//달리기 시스템을 누르고 있을때가 아니라 온오프로 변경

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;//캐릭터 최대 속도를 NormalSpeed로 설정
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))//Enhanced를 사용하겠다고 지정해주는 것
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
			//IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅(바꾼다는 뜻)
		{
			if (PlayerController->MoveAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(//UNFUNCTION을 하지 않으면 바인딩을 실패할 수 있음
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);
			}

			if (PlayerController->JumpAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Started,
					this,
					&ASpartaCharacter::SwitchSprint
				);
			}
		}
	}
}//여기까지가 키만 할당해주고 실제 동작은 없는 상태

//이 아래부터 실제로 함수가 무슨 작동을 하는지 작성
//FInputActionValue는 IA에셋에서 ValueType을 무엇을 설정했는가에 따라 값을 전달한다.
//Move는 Axis2D로 선택했기 때문에 FVector2D를 가져오고 Jump의 경우에는 bool값이기 때문에 bool을 가져온다.
void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;//컨트롤러가 있어야 방향 계산이 가능. 안전장치(유효성검사)

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
		//캐릭터가 바라보는 방향(정면)으로 X축 이동
		//AddMovementInput(방향, 크기)
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
		//캐릭터의 오른쪽 방향으로 Y축 이동
	}
}
void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();//Jump와 StopJumping는 기본 제공 함수
	}
}
void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}
void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();//마우스 역시 Axis2D로 지정해줬기 때문에 FVector2D를 가져온다.

	AddControllerYawInput(LookInput.X);//X축 기울기는 Yaw
	AddControllerPitchInput(LookInput.Y);//Y축 기울기는 Pitch
}
void ASpartaCharacter::SwitchSprint(const FInputActionValue& value)
{
	bIsSprinting = !bIsSprinting;//반대로 바꿔라
	if (GetCharacterMovement())//이동 컴포넌트가 null이 아닌지 확인(안전장치)
	{
		if (bIsSprinting)//true일때만 SprintSpeed로 사용
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		}
	}
}
