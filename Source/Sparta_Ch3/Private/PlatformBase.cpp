#include "PlatformBase.h"

APlatformBase::APlatformBase()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);
	StaticMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	//콜리전 충돌 설정을 강제적으로 설정해놓은 부모클래스
}


