#include "RotatePlatform.h"

ARotatePlatform::ARotatePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	RotationSpeed = 90.0f;
	MeshOffset = FVector::ZeroVector;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);

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
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}
}

