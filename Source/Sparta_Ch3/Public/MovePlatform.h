#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovePlatform.generated.h"

UCLASS()
class SPARTA_CH3_API AMovePlatform : public AActor
{
	GENERATED_BODY()
	
public:
	AMovePlatform();

protected:
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CoverMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovePlatform|Properties")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovePlatform|Properties")
	float MoveDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovePlatform|Properties", meta = (ClampMin = "-1.0", ClampMax = "1.0"))//meta(슬라이더만들기)
	FVector MoveDirection;
	FVector StartLocation;
	FVector EndLocation;
	bool bMovingForward;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
