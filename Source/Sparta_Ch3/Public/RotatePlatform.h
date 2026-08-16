#pragma once

#include "CoreMinimal.h"
#include "PlatformBase.h"
#include "RotatePlatform.generated.h"

UCLASS()
class SPARTA_CH3_API ARotatePlatform : public APlatformBase
{
	GENERATED_BODY()
	
public:	
	ARotatePlatform();
	FRotator GetPlatformRotation() const;//회전값을 내보낼 변수

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	FVector MeshOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float RotationSpeed;
	FRotator PlatformRotation;//실제 회전한 값을 담을 변수

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
