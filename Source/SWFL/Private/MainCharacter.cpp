// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Lightsaber.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create camera boom (pulls towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f; // Camera follows at this distance
	CameraBoom->SocketOffset = FVector(0.f, 75.f, 50.f); // Camera over the shoulder
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set radius and height for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(34.f, 88.f);

	// Create the following camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom
	FollowCamera->bUsePawnControlRotation = false; // Let only the boom adjust the orientation

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Do not rotate the character when the controller rotates
	// Affect just the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	bIsWalking = true;
	bShouldJog = false;
	bIsEvading = false;
	bIsDoubleStepping = false;
	bIsSprinting = false;

	SocketSpawnLeft = "lightsaber_l";
	SocketSpawnRight = "lightsaber_r";
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Ignore the camera for mesh and capsule
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Lightsaber_l = GetWorld()->SpawnActor<ALightsaber>(Lightsaber_1, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (Lightsaber_l)
	{
		Lightsaber_l->SetOwner(this);
		Lightsaber_l->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketSpawnLeft);
	}

	Lightsaber_r = GetWorld()->SpawnActor<ALightsaber>(Lightsaber_2, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (Lightsaber_r)
	{
		Lightsaber_r->SetOwner(this);
		Lightsaber_r->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketSpawnRight);
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::DoubleJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("ToggleLightsaber", IE_Pressed, this, &AMainCharacter::ToggleLightsaber);

	PlayerInputComponent->BindAction("ToggleMovement", IE_Pressed, this, &AMainCharacter::ToggleMovement);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::SprintOn);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::SprintOff);

	PlayerInputComponent->BindAction("Evade", IE_Pressed, this, &AMainCharacter::Evade);
	PlayerInputComponent->BindAction("DoubleStep", IE_DoubleClick, this, &AMainCharacter::DoubleStep);

	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &AMainCharacter::MeleeAttack);

	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &AMainCharacter::ForcePush);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

void AMainCharacter::MoveForward(float Value)
{
	if ((Controller) && (Value != 0.f))
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if ((Controller) && (Value != 0.f))
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::DoubleJump()
{
	if (GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();

		if (AnimInstance && DoubleJumpCounter <= 1)
		{
			switch (DoubleJumpCounter)
			{
			case 0:
				if (Lightsaber_l)
				{
					Lightsaber_l->GetTrail()->SetVisibility(true);
				}
				if (Lightsaber_r)
				{
					Lightsaber_r->GetTrail()->SetVisibility(true);
				}
				AMainCharacter::LaunchCharacter(JumpOneHeigth, false, true);
				DoubleJumpCounter++;
				if (FirstJump)
				{
					AnimInstance->Montage_Play(FirstJump, 1.f);
				}
				break;
			case 1:
				AMainCharacter::LaunchCharacter(JumpTwoHeigth, false, true);
				DoubleJumpCounter++;
				if (SecondJump)
				{
					AnimInstance->Montage_Play(SecondJump, 1.f);
				}
				break;
			default:
				break;
			}
		}
	}
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	DoubleJumpCounter = 0;
	if (Lightsaber_l)
	{
		Lightsaber_l->GetTrail()->SetVisibility(false);
	}
	if (Lightsaber_r)
	{
		Lightsaber_r->GetTrail()->SetVisibility(false);
	}
}

void AMainCharacter::Evade()
{
	if (!(bIsEvading))
	{
		if (!(this->GetCharacterMovement()->IsFalling()) && GetMesh()->GetAnimInstance())
		{
			UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();

			if (AnimInstance && EvadeMontage)
			{
				bIsEvading = true;

				if (bIsSprinting)
				{
					AnimInstance->Montage_Play(EvadeMontage, 1.25f);
				}
				else
				{
					AnimInstance->Montage_Play(EvadeMontage, 1.f);
				}

				if (Lightsaber_l)
				{
					Lightsaber_l->GetTrail()->SetVisibility(true);
				}
				if (Lightsaber_r)
				{
					Lightsaber_r->GetTrail()->SetVisibility(true);
				}
			}
		}
	}
}

void AMainCharacter::DoubleStep()
{
	if (!(this->GetCharacterMovement()->IsFalling()) && GetMesh()->GetAnimInstance() && !(bIsDoubleStepping))
	{
		UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();

		if (AnimInstance && DoubleStepMontage)
		{
			bIsDoubleStepping = true;
			AnimInstance->Montage_Play(DoubleStepMontage, 1.f);

			if (Lightsaber_l)
			{
				Lightsaber_l->GetTrail()->SetVisibility(true);
			}
			if (Lightsaber_r)
			{
				Lightsaber_r->GetTrail()->SetVisibility(true);
			}
		}
	}
}

void AMainCharacter::ToggleLightsaber()
{
	if (Lightsaber_r && Lightsaber_l)
	{
		if ((Lightsaber_l->GetIsIgnited() == false) && (Lightsaber_r->GetIsIgnited() == false))
		{
			Lightsaber_l->IgniteLightsaber();
			Lightsaber_r->IgniteLightsaber();
		}
		else
		{
			Lightsaber_l->ExtinguishLightsaber();
			Lightsaber_r->ExtinguishLightsaber();
		}
	}
}

void AMainCharacter::ToggleMovement()
{
	if (bIsSprinting)
	{
		return;
	}

	if (bIsWalking)
	{
		bIsWalking = false;
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
	else
	{
		bIsWalking = true;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void AMainCharacter::SprintOn()
{
	bIsSprinting = true;

	if (bIsWalking)
	{
		bIsWalking = true;
		bShouldJog = false;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		bIsWalking = false;
		bShouldJog = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AMainCharacter::SprintOff()
{
	bIsSprinting = false;

	if (bShouldJog)
	{
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void AMainCharacter::MeleeAttack()
{
	if (!(this->GetCharacterMovement()->IsFalling()) && Lightsaber_r && Lightsaber_l && (!bIsEvading) && (!bIsDoubleStepping))
	{
		if (Combo > 3)
		{
			Combo = 0;
		}

		if (bIsAttacking)
		{
			return;
		}

		if (GetMesh()->GetAnimInstance())
		{
			UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();

			if ((Lightsaber_l->GetIsIgnited() == false) && (Lightsaber_r->GetIsIgnited() == false))
			{
				ToggleLightsaber();
			}

			if (AnimInstance)
			{
				switch (Combo)
				{
				case 0:
					if (FirstSwing)
					{
						AnimInstance->Montage_Play(FirstSwing, 1.f);
					}
					bIsAttacking = true;
					break;
				case 1:
					if (FirstSwing)
					{
						AnimInstance->Montage_Play(SecondSwing, 1.f);
					}
					bIsAttacking = true;
					break;
				case 2:
					if (FirstSwing)
					{
						AnimInstance->Montage_Play(ThirdSwing, 1.f);
					}
					bIsAttacking = true;
					break;
				case 3:
					if (FirstSwing)
					{
						AnimInstance->Montage_Play(FourthSwing, 1.f);
					}
					bIsAttacking = true;
					break;
				default:
					break;
				}
			}

			Lightsaber_l->GetTrail()->SetVisibility(true);
			Lightsaber_r->GetTrail()->SetVisibility(true);
		}

		Combo++;
	}
}

void AMainCharacter::ForcePush()
{
	FHitResult OutHit;

	// Start point
	FVector StartPoint = GetActorLocation();

	// Get direction vector
	FVector ForwardVector = this->GetActorForwardVector();

	// Get end point based on blade's length
	FVector EndPoint = StartPoint + (ForwardVector * 500);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this->GetOwner());

	// Save hit result
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartPoint, EndPoint, ECC_Visibility, CollisionParams);

	DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Purple, false, 1, 0, 1);

	// Check if blade is colliding
	if (bIsHit)
	{
		//Get the static mesh of the chosen Actor
		UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(OutHit.Actor->GetRootComponent());

		//If the static mesh is valid apply the given force
		if (SM)
		{
			if (ForceVFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ForceVFX,
					OutHit.ImpactPoint,
					GetActorRotation(),
					FVector(0.5f),
					true,
					EPSCPoolMethod::AutoRelease,
					true
				);
			}

			SM->AddImpulse(ForwardVector * 2000.f * SM->GetMass());

			UE_LOG(LogTemp, Warning, TEXT("Yes"));
		}
	}
}
