// Fill out your copyright notice in the Description page of Project Settings.


#include "Lightsaber.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"

// Sets default values
ALightsaber::ALightsaber()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create hilt
	Hilt = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hilt"));
	RootComponent = Hilt;

	// Create blade, set its Z scale on 0 and adjust to distance to hilt by 10
	Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
	Blade->SetupAttachment(Hilt);
	Blade->SetRelativeScale3D(FVector(1, 1, 0));
	Blade->SetRelativeLocation(FVector(0, 0, 10));
	Blade->SetVisibility(false);

	// Construct collision for blade
	BladeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBox"));
	BladeCollision->SetupAttachment(Blade);

	// Add light on blade
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(Blade);
	Light->SetVisibility(false);

	// Add unstable look on blade
	Beam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("UnstableVFX"));
	Beam->SetupAttachment(Blade);
	Beam->SetVisibility(false);

	// Add unstable look on blade
	Trail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailVFX"));
	Trail->SetupAttachment(Blade);
	Trail->SetVisibility(false);

	// Add audio component to store the ignition sound and set its auto activation on false
	IgniteSound = CreateDefaultSubobject<UAudioComponent>(TEXT("IgniteSFX"));
	IgniteSound->SetupAttachment(Blade);
	IgniteSound->bAutoActivate = false;

	// Add audio component to store the idle sound and set its auto activation on false
	IdleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("IdleSFX"));
	IdleSound->SetupAttachment(Blade);
	IdleSound->bAutoActivate = false;

	// Add audio component to store the extinguishing sound and set its auto activation on false
	ExtinguishSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ExtinguishSFX"));
	ExtinguishSound->SetupAttachment(Blade);
	ExtinguishSound->bAutoActivate = false;

	// Set max Z scale for blade
	zMaxScale = 1.f;

	// Set interpolation speed for blade between 0 and max value
	zInterpSpeed = 8.f;

	// Set default light intensity
	DefaultLightIntensity = 500.f;

	// Bool tracking the state of the blade
	bIsIgnited = false;

	// Set hilt socket name
	HiltSocket = "IgniteVFX";
	BaseBladeSocket = "Base";
	CenterBladeSocket = "Center";
	TipBladeSocket = "Tip";
}

// Called when the game starts or when spawned
void ALightsaber::BeginPlay()
{
	Super::BeginPlay();

	// Bind functions to blade overlap
	BladeCollision->OnComponentBeginOverlap.AddDynamic(this, &ALightsaber::OnBladeOverlap);

	// Set blade collisions
	BladeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BladeCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BladeCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BladeCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ALightsaber::OnBladeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto MainCharacter = Cast<AMainCharacter>(OtherActor);
	
	if (GetOwner() && (MainCharacter != GetOwner()))
	{
		DoDamage(MainCharacter);
	}
}

void ALightsaber::ActivateBladeCollision()
{
	BladeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALightsaber::DeactivateBladeCollision()
{
	BladeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALightsaber::DoDamage(class AMainCharacter* Victim)
{
	if (Victim == nullptr)
	{
		return;
	}

	auto Character = Cast<AMainCharacter>(Victim);

	if (Character)
	{
		if (Character->GetHitSound())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Character->GetHitSound(), Character->GetActorLocation());
		}

		if (Character->GetHitVFX())
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, Character->GetHitVFX(), Character->GetActorLocation());
		}
	}
}

void ALightsaber::IgniteLightsaber()
{
	zInterpSpeed = 8.f;

	// Set desired Z scale for blade
	zScaleTarget = zMaxScale;

	// Set current state to ignited
	bIsIgnited = true;

	// If ignition sound is set, play it
	if (IgniteSound)
	{
		IgniteSound->Play(0.f);
	}

	// If idle sound is set, play it
	if (IdleSound)
	{
		IdleSound->Play(0.f);
	}

	// If ignition effect is set, spawn it
	if (IgniteVFX)
	{
		SpawnHiltVFX(IgniteVFX, Hilt, HiltSocket, FVector(0), FRotator(0), FVector(0.2));
	}

	// If trail effect is set, begin trail
	if (Trail)
	{
		Trail->BeginTrails(BaseBladeSocket, TipBladeSocket, ETrailWidthMode_FromCentre, 1.f);
	}
}

void ALightsaber::ExtinguishLightsaber()
{
	zInterpSpeed = 8.f;

	// Set desired Z scale for blade (reset it to base value)
	zScaleTarget = 0.f;

	// Set current state to extinguished
	bIsIgnited = false;

	// If extinguish sound is set, play it
	if (ExtinguishSound)
	{
		ExtinguishSound->Play(0.f);
	}

	// If idle sound is set, stop it
	if (IdleSound)
	{
		IdleSound->Stop();
	}

	// If extinguish effect is set, spawn it
	if (ExtinguishVFX)
	{
		SpawnHiltVFX(ExtinguishVFX, Hilt, HiltSocket, FVector(0), FRotator(0), FVector(0.2));
	}

	// If trail effect is set, end trail
	if (Trail)
	{
		Trail->EndTrails();
	}
}

void ALightsaber::SpawnHiltVFX(UParticleSystem* VFX, UStaticMeshComponent* Object, FName ObjectSocket, FVector VFXLocation, FRotator VFXRotation, FVector VFXScale)
{
	UGameplayStatics::SpawnEmitterAttached(
		VFX,
		Object,
		ObjectSocket,
		VFXLocation,
		VFXRotation,
		VFXScale,
		EAttachLocation::KeepRelativeOffset,
		true,
		EPSCPoolMethod::AutoRelease,
		true
	);
}

bool ALightsaber::RayCast(float& zCurrentScaleLimit, float& zCollisionScale)
{
	FHitResult OutHit;

	// Start point
	FVector StartPoint = Blade->GetSocketLocation("Base");

	// Get direction vector
	FVector ForwardVector = Blade->GetUpVector();

	// Get end point based on blade's length
	FVector EndPoint = StartPoint + (ForwardVector * zCurrentScaleLimit * engineBladeScale);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());

	// Save hit result
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartPoint, EndPoint, ECC_Visibility, CollisionParams);

	// Check if blade is colliding
	if (bIsHit)
	{
		// Update end point
		EndPoint = StartPoint + (ForwardVector * zCurrentScaleLimit * OutHit.ImpactPoint.Dist(StartPoint, OutHit.ImpactPoint));

		if (ExtinguishVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ExtinguishVFX,
				OutHit.ImpactPoint,
				GetActorRotation(),
				FVector(0.2f),
				true,
				EPSCPoolMethod::AutoRelease,
				true
			);
		}

		if (DecalMI)
		{
			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMI, FVector(15.f), OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation(), 2.f);
		}
	}

	// Draw debug ray
	// DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Purple, false, 1, 0, 1);

	// Set collision Z scale
	zCollisionScale = OutHit.ImpactPoint.Dist(StartPoint, EndPoint) / engineBladeScale;

	return bIsHit;
}

// Called every frame
void ALightsaber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Store blade's collision Z scale
	float zCollisionScale;

	// Track current scale on Z axis
	float zCurrentScale = FMath::FInterpTo(Blade->GetRelativeScale3D().Z, zScaleTarget, DeltaTime, zInterpSpeed);

	// If lightsaber is completely turned off, hide its components
	if (zCurrentScale <= 0.015f)
	{
		Light->SetVisibility(false);
		Blade->SetVisibility(false);
		Beam->SetVisibility(false);
		// Trail->SetVisibility(false);
	}
	else
	{
		Light->SetVisibility(true);
		Blade->SetVisibility(true);
		Beam->SetVisibility(true);
		// Trail->SetVisibility(true);

		// Check if the saber is colliding
		if (RayCast(zScaleTarget, zCollisionScale))
		{
			if (!bIsIgnited)
			{
				// Set interpolation speed to its default value
				zInterpSpeed = 8.f;
			}
			else
			{
				// Set high interpolation speed to remove the visual delay from scale interpolation
				zInterpSpeed = 1000.f;
			}

			// Light intensity interpolation based on collision scale
			Light->SetIntensity(zCollisionScale * DefaultLightIntensity);

			// Blade interpolation on Z axis based on collision scale
			Blade->SetRelativeScale3D(FVector(Blade->GetRelativeScale3D().X, Blade->GetRelativeScale3D().Y, zCollisionScale));
		}
		else
		{
			// Set interpolation speed to its default value
			zInterpSpeed = 8.f;

			// Light intensity interpolation
			Light->SetIntensity(zCurrentScale * DefaultLightIntensity);

			// Blade interpolation on Z axis
			Blade->SetRelativeScale3D(FVector(Blade->GetRelativeScale3D().X, Blade->GetRelativeScale3D().Y, zCurrentScale));
		}
	}
}