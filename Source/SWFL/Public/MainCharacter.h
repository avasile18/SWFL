// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class SWFL_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for forwards/backwards input
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	// Called via input to turn at a given rate
	// @param Rate is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// Called via input to look up/down at a given rate
	// @param Rate is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	void LookUpAtRate(float Rate);

	// Jump
	void DoubleJump();
	virtual void Landed(const FHitResult& Hit) override;

	// Evade
	void Evade();

	// DoubleStep
	void DoubleStep();

	// Switch the state of lightsaber
	void ToggleLightsaber();

	// Switch between run and jog
	void ToggleMovement();

	// Sprint
	void SprintOn();

	// Stop sprinting
	void SprintOff();

	void MeleeAttack();

	void ForcePush();

private:
	// Camera boom positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Following camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Base turn rates to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 225.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float JogSpeed = 375.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 600.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	int32 DoubleJumpCounter = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector JumpOneHeigth{0.f,0.f,300.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector JumpTwoHeigth{0.f, 0.f, 450.f};

	bool bIsWalking;
	bool bShouldJog;
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsEvading;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsDoubleStepping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class ALightsaber* Lightsaber_l;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	ALightsaber* Lightsaber_r;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALightsaber> Lightsaber_1;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALightsaber> Lightsaber_2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	FName SocketSpawnRight;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	FName SocketSpawnLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FirstSwing;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SecondSwing;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ThirdSwing;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FourthSwing;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UAnimMontage* FirstJump;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UAnimMontage* SecondJump;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UAnimMontage* EvadeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	UAnimMontage* DoubleStepMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 Combo = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ForceVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* HitSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* HitVFX;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	FORCEINLINE void SetIsAttacking(bool CanAttack) { bIsAttacking = CanAttack; }

	FORCEINLINE int32 GetCombo() const { return Combo; }
	FORCEINLINE void SetCombo(int32 ComboCount) { Combo = ComboCount; }

	FORCEINLINE class ALightsaber* GetLightsaberL() const { return Lightsaber_l; }
	FORCEINLINE class ALightsaber* GetLightsaberR() const { return Lightsaber_r; }

	FORCEINLINE USoundCue* GetHitSound() const { return HitSFX; }
	FORCEINLINE UParticleSystem* GetHitVFX() const { return HitVFX; }

	FORCEINLINE bool GetIsEvading() const { return bIsEvading; }
	FORCEINLINE void SetIsEvading(bool Evades) { bIsEvading = Evades; }

	FORCEINLINE bool GetIsDoubleStepping() const { return bIsDoubleStepping; }
	FORCEINLINE void SetIsDoubleStepping(bool DoubleSteps) { bIsDoubleStepping = DoubleSteps; }
};