// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lightsaber.generated.h"

/*
UENUM(BlueprintType)
enum class ECrystalType : uint8
{
	ECT_ORANGE UMETA(DisplayName = "Orange kyber crystal"),
	ECT_BLUE UMETA(DisplayName = "Blue kyber crystal"),
	ECT_RED UMETA(DisplayName = "Red kyber crystal"),
	ECT_PURPLE UMETA(DisplayName = "Purple kyber crystal"),

	ECT_MAX UMETA(DisplayName = "DefaultMAX")
};
*/

UCLASS()
class SWFL_API ALightsaber : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightsaber();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBladeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Activate/deactivate collision for blade
	UFUNCTION(BlueprintCallable)
	void ActivateBladeCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateBladeCollision();

	void DoDamage(class AMainCharacter* Victim);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	float zMaxScale;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	float zScaleTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	float zInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	float DefaultLightIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	bool bIsIgnited;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	FName HiltSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	FName TipBladeSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	FName CenterBladeSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	FName BaseBladeSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Body", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Hilt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Body", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Blade;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* Light;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* IgniteVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExtinguishVFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* Beam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* Trail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	class ADecalActor* Decal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | VFX", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* DecalMI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | SFX", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* IgniteSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | SFX", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* IdleSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | SFX", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ExtinguishSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	float engineBladeScale = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Tweaks", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BladeCollision;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void IgniteLightsaber();
	void ExtinguishLightsaber();

	void SpawnHiltVFX(UParticleSystem* VFX, UStaticMeshComponent* Object, FName ObjectSocket, FVector VFXLocation, FRotator VFXRotation, FVector VFXScale);

	bool RayCast(float& zCurrentScaleLimit, float& zCollisionScale);

	FORCEINLINE bool GetIsIgnited() const { return bIsIgnited; }

	FORCEINLINE UParticleSystemComponent* GetTrail() const { return Trail; }
};
