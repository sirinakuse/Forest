#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpartaPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
class AMovePlatform;
class ARotatePlatform;
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

	AMovePlatform* MovePlatform;
	ARotatePlatform* RotatePlatform;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Debug용
	bool bDebugPrinted = false;

	//Jump관련,중력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float JumpVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float GravityStrength;//중력값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
	float VerticalVelocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsGrounded;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float GroundCheckDistance;//Jump 애니메이션이 이상하기 때문에 바닥을 검사할 때 발밑을 얼마나 더 검사할지 추가해주는 용도

	//경사면, Move관련
	UPROPERTY(EditAnywhere, Category = "Movemnet")
	float MaxWalkableSlopeAngle;
	FVector PlayerMove;

	//Look관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	float MouseSensitivity;
	float CumulativePitch;
	float CumulativeYaw;
	FRotator DisplayRotation;
	float LerpSpeed;

	//달리기 속도를 만들어주기 위해 새로 변수를 추가한다. 기본속도와 "기본 속도" 대비 몇 배로 빠르게 달릴지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")//둘 다 에디터에서 수정 가능 하도록 만들어준다.
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;//몇 배로 빠르게 달릴지
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")//값이 전부 보이지만 수정 불가 블루프린트 역시 보기만 가능
	//float SprintSpeed;//실제 스프린트 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float GroundSpeed;

	bool bIsSprinting;//달리기를 온오프로 변경하기 위한 변수

	FVector PreviousLocation;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	//bool bHasMoveInput;//움직이는 입력을 받고 있는가. CharacterMovement에서 하던 작업을 내 코드 이동 로직 기준으로 변경하는 작업을 해야함

	//Enhanced Input에서 액션 값은 FInputActionValue로 전달
	UFUNCTION()
	void Move(const FInputActionValue& value);
	/*UFUNCTION()
	void StopMove(const FInputActionValue& value);*/
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void SwitchSprint(const FInputActionValue& value);
};
