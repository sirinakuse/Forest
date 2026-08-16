#include "SpartaPawn.h"
#include "SpartaPlayerController.h"
#include "MovePlatform.h"
#include "RotatePlatform.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASpartaPawn::ASpartaPawn()
{
	PrimaryActorTick.bCanEverTick = true;

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

	bUseControllerRotationYaw = true;
	//카메라(스프링암)이 돌면 캐릭터도 같이 돌린 방향을 보게 만듦

	LerpSpeed = 13.0f;
	//보간 속도(현재 Look함수에서 사용함)
	MouseSensitivity = 3.0f;

	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	bIsSprinting = false;
	//bHasMoveInput = false;

	JumpVelocity = 600.0f;
	GravityStrength = 980.f;
	VerticalVelocity = 0.0f;
	bIsGrounded = true;
	MaxWalkableSlopeAngle = 45.0f;
	GroundSpeed = 0.0f;
}

void ASpartaPawn::BeginPlay()
{
	Super::BeginPlay();

	FRotator StartRotation = GetActorRotation();
	PreviousLocation = GetActorLocation();//바로 직전에 캐릭터가 있던 위치를 기억해두는 장소
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
	Super::Tick(DeltaTime);

	//전체적인 순서: 중력->속도->이동량->이동 예정 위치->현재 발 위치->발 이동 예정 위치

	VerticalVelocity -= GravityStrength * DeltaTime;
	//시간이 지나면서 중력을 받는다. 음수로 계속 떨어지는게 정상 상태임. 중력 계산하는 부분.
	float VerticalMovement = VerticalVelocity * DeltaTime;
	//VerticlaVelocity는 속도이고 VerticalMovement는 이번 프레임에 움직일 거리를 계산하는 것이다.
	FVector PlatformMovement = FVector::ZeroVector;

	FVector CurrentLocation = GetActorLocation();
	//엑터의 현재 위치를 저장. 캐릭터 중심 위치(캡슐이 Root라 동일함)

	FVector NextLocation = CurrentLocation;
	NextLocation.Z += VerticalMovement;
	//현재 위치에서 이번 프레임의 이동량을 적용했을 때 도착할 "예정 위치"

	float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
	//캡슐 반높이 위치를 가져옴

	FHitResult Hit;//충돌 결과를 담을 변수. 충돌이 발생했다면 그 결과를 여기에 담는다.

	FCollisionQueryParams CollisionParams;
	//Sweep을 어떻게 검사할지 설정하는 옵션
	CollisionParams.bTraceComplex = false;
	//복잡한 Collision을 사용할 수 있도록 설정한다 = true, 단순한 Collision일 경우 = false

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,//충돌 결과
		CurrentLocation,//Sweep 시작 위치
		NextLocation,//Sweep 종료 위치
		CapsuleComp->GetComponentQuat(),//캡슐의 실제 회전 그대로 사용
		ECC_WorldStatic,//뭐랑 충돌하는지?
		CapsuleComp->GetCollisionShape(0),
		//실제 캐릭터 캡슐의 CollisionShape 그대로 사용. 여기서 받는 매개변수는 현재 사용하는 충돌 모양 그대로 가져와라(입력 안하면 여전히 이상함)
		CollisionParams//검사 설정
	);

	if (bHit && VerticalVelocity <= 0.0f)
	{
		bIsGrounded = true;
		VerticalVelocity = 0.0f;
	
		//사이에 지면이나 다른 World Static 충돌체를 발견하면 실행

		MovePlatform = Cast<AMovePlatform>(Hit.GetActor());//지금 밟고 있는 발판이 움직이는 발판인지 캐스트
		RotatePlatform = Cast<ARotatePlatform>(Hit.GetActor());//지금 밟고 있는 발판이 회전 발판인지 캐스트

		if (MovePlatform)//부딪힌게 MovePlatform이면 실행
		{
			PlatformMovement = MovePlatform->GetPlatformMovement();//플랫폼이 움직인값을 실제로 가져옴
			SetActorLocation(GetActorLocation() + PlatformMovement);
		}

		if (RotatePlatform)
		{
			FRotator PlatformRotation = RotatePlatform->GetPlatformRotation();//회전값을 실제로 가져옴
			FVector PlatformCenter = RotatePlatform->GetActorLocation();
			//회전발판의 Actor위치를 가져옴. Actor위치가 발판의 회전 중심축이기 때문.
			//위치값이기 때문에 FVector를 자료형으로 사용해야함 FRotator로 받아오면 오류남.
			FVector RelativeLocation = CurrentLocation - PlatformCenter;
			//상대 위치 계산 캐릭터의 현재 위치에서 회전하는 발판의 위치를 빼면 발판의 회전 중심으로부터 캐릭터가 어느 방향으로, 얼마나 떨어져 있는지 계산한다.

			FVector RotatedRelativeLocation = PlatformRotation.RotateVector(RelativeLocation);
			//발판 중심 기준 상대 위치를 발판이 이번 프레임 회전한 만큼 돌린 결과를 저장. 캐릭터가 발판을 따라 움직이진 않는다.
			//다음 프레임에 캐릭터가 어디 있어야하는 지 미리 계산한 값
			//월드 위치가 아니기 때문에 월드 위치를 다시 계산해줘야 한다.

			FVector NewLocation = PlatformCenter + RotatedRelativeLocation;
			//실제 도착하는 좌표(월드 좌표)

			SetActorLocation(NewLocation);
			//캐릭터가 실제로 움직이게 지정
		}
	
	}
	else
	{
		bIsGrounded = false;
		SetActorLocation(NextLocation);
		//아무것도 못 찾으면 예정 위치로 실제 이동
	}

	//FVector ActualMove = CurrentLocation - PreviousLocation;
	////이번 프레임에 실제로 얼마나 이동했는가.
	////이전 위치와 지금 위치를 비교해서 실제 이동량을 구함
	//FVector CharacterMovement = ActualMove - PlatformMovement;
	GroundSpeed = PlayerMove.Size2D() / DeltaTime;

	PlayerMove = FVector::ZeroVector;

	PreviousLocation = CurrentLocation;
	//계산이 끝나면 이전 위치 저장 값을 현재 위치 값으로 다시 바꿔서 저장해줌

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
			//if (PlayerController->MoveAction)
			//{
			//	//IA_Move 액션 키를 "키를 누르고 있는 동안" Move()를 호출
			//	EnhancedInput->BindAction(//UNFUNCTION을 하지 않으면 바인딩을 실패할 수 있음
			//		PlayerController->MoveAction,
			//		ETriggerEvent::Completed,
			//		this,
			//		&ASpartaPawn::StopMove
			//	);
			//}

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
	//Move의 입력값을 가져온다.
	//IA_Move의 Value Type을 Axis2D로 설정했기 때문에 FVector2D로 받는다.
	//bHasMoveInput = !MoveInput.IsNearlyZero(); //이동로직을 내가 직접 관리할때 사용했음. Tick으로 자동으로 관리하는 걸로 변경함

	float CurrentSpeed = 0.0f;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	//현재 월드(레벨)의 이번 프레임에 흐른 시간(초)를 얻어온다.
	//이동량에 DeltaTime을 곱해 프레임에 관계 없이 일정한 속도로 이동하도록 한다.


	if (bIsSprinting)
	{
		CurrentSpeed = SprintSpeed;
	}
	else
	{
		CurrentSpeed = NormalSpeed;
	}
	//달리기 상태에 따라 이번 이동에 사용할 속도를 결정한다.
	//Move를 만들때 기본 속도를 알아서 지정할 수 있도록 에디터에서 보이게 빼줬기 때문에 에디터에서 지정을 안해주면 폰이 움직이지 않는다. 주의!

	FVector CurrentMove = FVector(MoveInput.X * CurrentSpeed * DeltaTime, MoveInput.Y * CurrentSpeed * DeltaTime, 0.0f);
	//이번 프레임에 이동할 양을 계산한다. Local기준 이동량
	//MoveInput은 캐릭터의 Local 기준 방향이므로 현재 캐릭터 기준으로 이동량이 만들어진다.


	FVector CurrentLocation = GetActorLocation();
	//액터의 현재 위치를 가져옴

	FVector WorldMove = GetActorTransform().TransformVectorNoScale(CurrentMove);
	//Local 이동량을 World 기준 이동량으로 변환한다.
	//캐릭터가 바라보는 방향에 따라 실제 월드에서 이동할 방향이 결정된다.

	PlayerMove = WorldMove;
	//발판 이동과는 무관하게 플레이어가 직접 만든 이동량만 따로 가지고 있는 변수

	FVector NextLocation = CurrentLocation + WorldMove;
	//현재 위치에 이번 프레임의 이동량을 더해, 충돌이 없다면 도착하게 될 다음 위치를 미리 계산한다.

	FHitResult Hit;//충돌 정보 저장

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		CurrentLocation,
		NextLocation,
		CapsuleComp->GetComponentQuat(),
		ECC_WorldStatic,
		CapsuleComp->GetCollisionShape(0),
		CollisionParams
	);//내 실제 CapsuleComponent와 똑같은 캡슐이 움직였을 때 World Static과 부딪히는지 확인하는 것.
	if (!bHit)//부딪히지 않았을 때
	{
		SetActorLocation(NextLocation);
		//충돌이 없다면 예정된 다음 위치로 이도한다.
	}
	else//무언가에 부딪혔을 때
	{
		float SlopeAngle = FMath::RadiansToDegrees(
			FMath::Acos(Hit.ImpactNormal.Z)
			//Hit.ImpactNormal.Z: 충돌한 면의 법선 벡터의 Z값이라고 하는데 Z가 1에 가까울수록 평평한 바닥이고, 0에 가까울수록 가파른 면이다.
			//Acos: 아크코사인. Hit.ImpactNormal.Z값을 가지고 각도를 계산해주는 함수다. 결과값은 각도(Radian)가 나온다.
			//FMath::RadiansToDegrees: 실제 사람이 알아보기 쉬운 각도로 변환시켜주는 것.
			//지금 부딪힌 면이 수평 바닥에서부터 몇 도 기울어져 있는지 계산해서 대입해줘라.
		);
		if (SlopeAngle <= MaxWalkableSlopeAngle)//충돌한 면의 경사각이 최대 허용 경사각 이하인지 판단한다.
		{
			FVector SlideMove = FVector::VectorPlaneProject(//이 벡터를 이 평면 위에 놓으면 어느 방향이 되는가.
				WorldMove,//원래 가려고 했던 방향
				Hit.ImpactNormal//부딪힌 경사면이 바라보는 방향
			);//그 경사면을 따라 움직일 수 있는 방향
			//원래 이동 방향을 충돌한 경사면의 평면에 투영한다.
			//이를 통해 경사면을 따라 이동할 수 있는 방향을 계산한다.

			FVector SlideLocation = Hit.Location + SlideMove;//충돌한 위치에서 경사면을 따라 이동한 위치
			SetActorLocation(SlideLocation);//계산된 경사면 방향의 위치로 이동한다.
		}

		else
		{
			//걸을 수 없는 경사 == 벽처럼 처리
			FVector WallNormal = Hit.ImpactNormal;
			WallNormal.Z = 0.0f;
			WallNormal.Normalize();

			FVector SlideMove = FVector::VectorPlaneProject(
				WorldMove,
				WallNormal
			);

			FVector Slidelocation = CurrentLocation + SlideMove;
			SetActorLocation(Slidelocation);
			//최대 경사각을 초과한 가파른 면이라면 충돌한 위치에서 멈춘다.
			//Hit.Location을 사용하면 최대경사면에서 움직일 수 없는 상태가 된다.
			//SetActorLocation(CurrentLocation)만 사용하면 가파른 경사면에 갇히는 문제가 발생함
		}
	}

	//FVector ActualMove = GetActorLocation() - CurrentLocation;
	//이번 프레임에 실제로 얼마나 이동했는가.

	//GroundSpeed = ActualMove.Size2D() / DeltaTime;
	//.Size2D() X/Y 이동거리만 계산하는 함수
	//이동하는 로직이 여러개라 맨 마지막에 추가해줘야 정상적으로 계산됨
	//Tick으로 옮김
}

//void ASpartaPawn::StopMove(const FInputActionValue& value)
//{
//	bHasMoveInput = false;
//	GroundSpeed = 0.0f;
//}

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
	if (FMath::Abs(LookInput.Y) > 0.18f)//데드존 만들기, 마우스가 가로로만 움직이고 싶은데 세로로 따라오는 걸 방지함(손떨림 방지)
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

