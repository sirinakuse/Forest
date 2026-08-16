#include "RotatePlatform.h"

ARotatePlatform::ARotatePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	RotationSpeed = 90.0f;
	MeshOffset = FVector::ZeroVector;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/ThirdParty/DreamscapeSeries/DreamscapeTower/Meshes/Structures/SM_Bridge_02.SM_Bridge_02'"));

	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}
}

void ARotatePlatform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (StaticMeshComp)
	{
		StaticMeshComp->SetRelativeLocation(MeshOffset);
	}
}

void ARotatePlatform::BeginPlay()
{
	Super::BeginPlay();	
}

void ARotatePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!FMath::IsNearlyZero(RotationSpeed))
	{
		FRotator RotationDelta(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));//이번 프레임에 실제로 회전 시킬 양
		AddActorLocalRotation(RotationDelta);//실제로 더한상태
		PlatformRotation = RotationDelta;//회전 시킨 값을 저장해두는 변수

	}
}

FRotator ARotatePlatform::GetPlatformRotation() const
{
	return PlatformRotation;
}
