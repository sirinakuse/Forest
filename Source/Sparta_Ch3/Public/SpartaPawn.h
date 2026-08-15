#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpartaPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
//Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class SPARTA_CH3_API ASpartaPawn : public APawn
{
	GENERATED_BODY()

public:
	ASpartaPawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skeletal")
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Look관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	float MouseSensitivity;
	float CumulativePitch;
	float CumulativeYaw;

	//달리기 속도를 만들어주기 위해 새로 변수를 추가한다. 기본속도와 "기본 속도" 대비 몇 배로 빠르게 달릴지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")//둘 다 에디터에서 수정 가능 하도록 만들어준다.
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;//몇 배로 빠르게 달릴지

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")//값이 전부 보이지만 수정 불가 블루프린트 역시 보기만 가능
	float SprintSpeed;//실제 스프린트 속도

	bool bIsSprinting;//달리기를 온오프로 변경하기 위한 변수

	//Enhanced Input에서 액션 값은 FInputActionValue로 전달
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void SwitchSprint(const FInputActionValue& value);
};
