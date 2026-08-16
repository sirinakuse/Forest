#pragma once

#include "CoreMinimal.h"
#include "PlatformBase.h"
#include "MovePlatform.generated.h"

UCLASS()
class SPARTA_CH3_API AMovePlatform : public APlatformBase
{
	GENERATED_BODY()
	
public:
	AMovePlatform();
	FVector GetPlatformMovement() const;//발판 이동량을 외부에서 알 수 있게 getter로 만들어줌

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CoverMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovePlatform|Properties")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovePlatform|Properties")
	float MoveDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovePlatform|Properties", meta = (ClampMin = "-1.0", ClampMax = "1.0"))//meta(슬라이더만들기)
	FVector MoveDirection;
	FVector PlatformMovement;//이동량을 담을 변수
	FVector StartLocation;
	FVector EndLocation;
	bool bMovingForward;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
