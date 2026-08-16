#include "MovePlatform.h"

AMovePlatform::AMovePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	MoveSpeed = 150.0f;
	MoveDistance = 0.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/ThirdParty/DreamscapeSeries/DreamscapeTower/Meshes/Tower/SM_Tower_Base.SM_Tower_Base'"));

	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
		SetActorScale3D(FVector(0.4f));
	}

	CoverMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoverStaticMesh"));//여러개를 사용 할 때 겹치면 안됨
	CoverMeshComp->SetupAttachment(SceneRoot);
}

void AMovePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();//시작 위치값을 월드기준으로 가져옴
	EndLocation = StartLocation + MoveDirection.GetSafeNormal() * MoveDistance;//MoveDirection은 입력한 방향으로 움직이게 만들어주는 용도 GetSafeNormal은 방향 벡터의 길이를 정확히 1로 고정하는 것
	bMovingForward = true;//현재 이동 방향(true: EndLocation 방향, false: StartLocation 방향) 판단용 변수
}

void AMovePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Movement = MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime;
	//MoveDirection이 가리키는 방향으로, MoveSpeed 속도로, 이번 프레임(DeltaTime) 동안 이동할 거리를 계산
	//계산한 값을 변수에 넣어줌
	

	if (bMovingForward)
	{
		AddActorWorldOffset(Movement);//엑터에 로테이션 값이 들어가 있으면 방향이 이상해질 수 있기 때문에 월드를 기준으로 바꿨다.
		PlatformMovement = Movement;//실제로 발판이 이동한 값을 저장해서 외부에서 사용할 값으로 쓸 예정이다.
	}
	else
	{
		AddActorWorldOffset(-Movement);
		PlatformMovement = -Movement;
	}

	if (FVector::Dist(GetActorLocation(), EndLocation) < 5.0f)
	{
		bMovingForward = false;// EndLocation 도착 → 방향 전환(되돌아가기)
	}
	else if (FVector::Dist(GetActorLocation(), StartLocation) < 5.0f)
	{
		bMovingForward = true;// StartLocation 복귀 → 다시 정방향으로
	}

}

FVector AMovePlatform::GetPlatformMovement() const//반환형이 FVector이기 때문에 FVector를 써줌
{
	return PlatformMovement;//발판 이동값 저장한 것을 리턴
}

