#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
//Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class SPARTA_CH3_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	//달리기 속도를 만들어주기 위해 새로 변수를 추가한다. 기본속도와 "기본 속도" 대비 몇 배로 빠르게 달릴지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")//둘 다 에디터에서 수정 가능 하도록 만들어준다.
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;//몇 배로 빠르게 달릴지

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")//값이 전부 보이지만 수정 불가 블루프린트 역시 보기만 가능
	float SprintSpeed;//실제 스프린트 속도

	bool bIsSprinting;//달리기를 온오프로 변경하기 위한 변수

	//입력 바인딩을 처리할 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
