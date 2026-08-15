#include "SpartaPawn.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASpartaPawn::ASpartaPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(false);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;//컨트롤러 움직일때 카메라가 같이 안 움직이게 하기 위함(스피링 암이 방향 따라가게 적용 해놨음)

	MouseSensitivity = 3.0f;

	bIsSprinting = false;

}

void ASpartaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&ASpartaPawn::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::StartJump
				);
			}

			if (PlayerController->JumpAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaPawn::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Started,
					this,
					&ASpartaPawn::SwitchSprint
				);
			}
		}
	}
}//여기까지가 키만 할당해주고 실제 동작은 없는 상태
//키할당은 controller를 설정해주는 것이기 때문에 Character과 동일하게 써도 된다.

//이 아래부터 실제로 함수가 무슨 작동을 하는지 작성
//FInputActionValue는 IA에셋에서 ValueType을 무엇을 설정했는가에 따라 값을 전달한다.
void ASpartaPawn::Move(const FInputActionValue& value)
{
}

void ASpartaPawn::StartJump(const FInputActionValue& value)
{
}

void ASpartaPawn::StopJump(const FInputActionValue& value)
{
}

void ASpartaPawn::Look(const FInputActionValue& value)
{
	const FVector2D LookInput = value.Get<FVector2d>();
	FRotator CurrentRotation = GetController()->GetControlRotation();
	//Current(현재의) CurrentRotaion이라는 변수를 지역변수로 지정하고 컨트롤러의 회전값을 가져온다.
	FRotator NewRotation = CurrentRotation;
	NewRotation.Yaw += LookInput.X * MouseSensitivity;
	NewRotation.Pitch += LookInput.Y * MouseSensitivity;
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -80.0f, 80.0f);//FMath::Clamp는 제한하려는 대상과 자료형을 맞춰주는게 좋다.
	//화면을 세로로 움직일 경우 범위 제한을 둬야 한다. 범위 제한이 없을경우 화면이 이상하게 뒤집힌다.
	GetController()->SetControlRotation(NewRotation);
	//GetController로 현재 폰을 컨트롤 하고 있는 컨트롤러를 가져와야한다.
	//SetControlRotation은 컨트롤러를 호출해야 쓸 수 있는 함수... 잊지 말기...
	//문제가 생겼다. 누적값때문에 Pitch가 올라오거나 내려가는게 늦어진다.
}

void ASpartaPawn::SwitchSprint(const FInputActionValue& value)
{
}

