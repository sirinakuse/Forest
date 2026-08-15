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

	LerpSpeed = 13.0f;
	//보간 속도(현재 Look함수에서 사용함)
	MouseSensitivity = 3.0f;

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	bIsSprinting = false;

	JumpVelocity = 600.0f;
	GravityStrength = 980.f;
	VerticalVelocity = 0.0f;
	bIsGrounded = true;
}

void ASpartaPawn::BeginPlay()
{
	Super::BeginPlay();

	FRotator StartRotation = GetActorRotation();
	//현재 이 폰의 회전값을 저장한다.
	CumulativeYaw = StartRotation.Yaw;
	//CumulativeYaw는 float이기 때문에 현재 이 폰의 회전값을 따로 저장을 해주고 거기서 Yaw값만 뽑아내서 쓰는것이다.
	FRotator CurrentControlRotation = GetController()->GetControlRotation();
	//APawn자체에도 GetControlRotation함수가 정의 되어 있고 내부적으로 자기컨트롤러를 찾아서 회전 값을 반환해주는 함수다.
	//명시적으로 GetController()->GetControlRotation()으로 사용함.
	CumulativePitch = CurrentControlRotation.Pitch;
	//Pitch는 수평선을 바라보는걸 초기값으로 넣을 경우에는 0으로 지정한다.
	//Pitch를 카메라 기준으로 설정해 줄 수도 있다.(현재는 카메라가 스피링 암에 달려있기 때문에 카메라가 컨트롤러의 값을 그대로 이어받는 구조다.)
	//이럴 경우에는 개념적으로 더 정확한 소스는 컨트롤러다.
	DisplayRotation = FRotator(CumulativePitch, CumulativeYaw, 0.0f);
	//Yaw는 GetActorRotation으로 받지만 Pitch의 경우에는 컨트롤러에서 받기 때문에 초기값에 GetActorRotation을 넣으면 Pitch의 값은 월드 기준으로 0으로 가버려서 마우스가 튈 수 있음.
}

void ASpartaPawn::Tick(float DeltaTime)
{
	VerticalVelocity -= GravityStrength * DeltaTime;
	AddActorWorldOffset(FVector(0.0f, 0.0f, VerticalVelocity * DeltaTime));
	//움직임이 있을 때는 무조건 DeltaTime을 곱해줘야 프레임 독립성(120프레임이든 30프레임이든 동일한 거리를 움직이게 해주는 것)이 생긴다.
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
	const FVector2D MoveInput = value.Get<FVector2D>();
	//Move의 입력값 뽑기. IA_Move에서 Axis2D로 설정을 해놨기 때문에 FVector2D로 뽑아서 사용
	//Look의 경우에는 FVetor로 뽑아서 FRotator를 사용하지만 Move같은 경우에는 FVector를 계속 사용한다.
	float CurrentSpeed = 0.0f;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	//현재 월드(레벨)의 이번 프레임에 흐른 시간(초)를 얻어온다.
	if (bIsSprinting)
	{
		CurrentSpeed = SprintSpeed;
	}
	else
	{
		CurrentSpeed = NormalSpeed;
	}
	//Move를 만들때 기본 속도를 알아서 지정할 수 있도록 에디터에서 보이게 빼줬기 때문에 에디터에서 지정을 안해주면 폰이 움직이지 않는다. 주의!
	FVector CurrentMove = FVector(MoveInput.X * CurrentSpeed * DeltaTime, MoveInput.Y * CurrentSpeed * DeltaTime, 0.0f);
	AddActorLocalOffset(CurrentMove);
}

void ASpartaPawn::StartJump(const FInputActionValue& value)
{
	if (bIsGrounded)
	{
		VerticalVelocity = JumpVelocity;
		bIsGrounded = false;
	}
}

void ASpartaPawn::StopJump(const FInputActionValue& value)
{
	if (VerticalVelocity > 0.0f)
	{
		VerticalVelocity = VerticalVelocity * 0.5f;
	}
}

void ASpartaPawn::Look(const FInputActionValue& value)
{
	const FVector2D LookInput = value.Get<FVector2d>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	//이전에는 NewRotation이라는 임시 변수에 넣어줬는데 지금은 멤버변수로 회전값을 저장해 두었기 때문에 임시 변수 자체가 필요가 없어진다.
	CumulativeYaw += LookInput.X * MouseSensitivity;
	if (FMath::Abs(LookInput.Y) > 0.15f)//데드존 만들기, 마우스가 가로로만 움직이고 싶은데 세로로 따라오는 걸 방지함(손떨림 방지)
	{
		CumulativePitch += LookInput.Y * MouseSensitivity;
	}
	CumulativePitch = FMath::Clamp(CumulativePitch, -80.0f, 80.0f);
	//FMath::Clamp는 제한하려는 대상과 자료형을 맞춰주는게 좋다.
	//SetControlRotation은 FRotator하나를 통째로 받는 함수이기 때문에 분리해서 만든 Pitch와 Yaw를 하나로 합치는 작업을 해줘야한다.
	FRotator CombineRotation = FRotator(0.0f, 0.0f, 0.0f);
	//FRotator를 사용할때는 받는 값도 FRotator로 받아야 한다! 주의 할 것! (0.0f, 0.0f, 0.0f)만 쓰면 오류남
	CombineRotation.Pitch = CumulativePitch;
	CombineRotation.Yaw = CumulativeYaw;
	//위 세 줄을 줄여서 FRotator CombineRotation = FRotator(CumulativePitch, CumulativeYaw, 0.0f)라고 줄여서 쓸 수 있다.
	//Rotator의 순서는 Pitch(Y), Yaw(X), Roll(Z)다.
	/*DisplayRotation = FMath::RInterpTo(DisplayRotation, CombineRotation, DeltaTime, LerpSpeed);//카메라 전환 시 부드러운 회전 같은 걸 사용하고 싶을때 사용(현재는 사용안함)*/
	//FMath::RInterpTo()는 매개변수로 "현재값, 목표값, DeltaTime, 보간속도"를 받고 계산된 새로운 "현재값"을 결과로 돌려준다.
	GetController()->SetControlRotation(CombineRotation);
}

void ASpartaPawn::SwitchSprint(const FInputActionValue& value)
{
	
}

