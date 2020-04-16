// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MainPlayerController.h"
#include "MainAnimInstance.h"
#include "Inventory.h"
#include "CharacterCombat.h"

// Sets default values
AMain::AMain()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create Camera Boom (pulls towards the player if there is a collision) */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set Size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(27.0f, 130.0f);


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;
	OrientRotationRate = 540.0f;
	JumpVelocity = 650.0f;
	AirControl = 0.2f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, OrientRotationRate, 0.0f);
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->AirControl = AirControl;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	MeleeCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeCollider"));
	MeleeCollider->SetupAttachment(GetRootComponent());

	CharacterInventory = CreateDefaultSubobject<UInventory>(TEXT("Backpack"));
	CharacterCombat = CreateDefaultSubobject<UCharacterCombat>(TEXT("CombatManager"));

	MaxHealth = 100.0f;
	MaxStamina = 150.0f;
	Coins = 0;

	RunningSpeed = 650.0f;
	SprintingSpeed = 950.0f;

	bShiftKeyDown = false;
	bAttacking = false;
	bAttackButtonDown = false;

	//Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.0f;
	MinSprintStamina = 50.0f;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPC = Cast<AMainPlayerController>(GetController());

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	MainAnimInstance = Cast<UMainAnimInstance>(AnimInstance);

	bIsAlive = true;

	Health = MaxHealth;
	Stamina = MaxStamina;
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsAlive)
		return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	float MovementSpeed = this->GetVelocity().Size();

	if(bShiftKeyDown && !bAttackButtonDown)
	{
		if(MovementSpeed > 0.0f)
		{
			if(MovementStatus == EMovementStatus::EMS_Sprinting)
			{
				if(!CharacterInventory->bGetIsSheathed())
					CharacterInventory->SheatheEquipment();

				Stamina -= DeltaStamina;
			}

			if(StaminaStatus == EStaminaStatus::ESS_Normal)
			{
				if(Stamina - DeltaStamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				}
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else if(StaminaStatus == EStaminaStatus::ESS_BelowMinimum)
			{
				if(Stamina - DeltaStamina <= 0.0f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0.0f;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
			}
			else if(StaminaStatus == EStaminaStatus::ESS_Exhausted)
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			}
			else if(StaminaStatus == EStaminaStatus::ESS_ExhaustedRecovering)
			{
				if(Stamina + DeltaStamina >= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
				}
			}
		}
		else
			SetMovementStatus(EMovementStatus::EMS_Normal);
	}

	if(MovementStatus != EMovementStatus::EMS_Sprinting)
	{
		if(Stamina + DeltaStamina >= MaxStamina)
		{
			Stamina = MaxStamina;
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		else if(Stamina + DeltaStamina > MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
			Stamina += DeltaStamina;
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtRotationYaw;
}

void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AMain::Interact);
	PlayerInputComponent->BindAction(TEXT("MoveWeapon"), IE_Pressed, this, &AMain::MoveWeapon);

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AMain::Attack);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &AMain::StopAttacking);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMain::LookUpAtRate);
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if(MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::Jump()
{

	if(!bAttacking && bIsAlive)
	{
		ACharacter::Jump();
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
	SetMovementStatus(EMovementStatus::EMS_Normal);
}

void AMain::MoveForward(float Value)
{
	if(!bIsAlive)
		return;

	if(Controller != nullptr && Value != 0.0f && !bAttacking)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value)
{
	if(!bIsAlive)
		return;

	if(Controller != nullptr && Value != 0.0f && !bAttacking)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::TurnAtRate(float Rate)
{
	if(!bIsAlive)
		return;

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::Interact()
{
	if(!bIsAlive)
		return;
}

void AMain::Attack()
{
	if(!bIsAlive)
		return;

	bAttackButtonDown = true;

 	bool bIsFalling = GetCharacterMovement()->IsFalling();
	if(!bIsFalling)
	{
		CharacterCombat->StartAttack();
	}
}

void AMain::StopAttacking()
{
	bAttackButtonDown = false;
}

void AMain::MoveWeapon()
{
	if(!bIsAlive)
		return;

	if(MainAnimInstance->GetAnimationState() == EAnimationState::EAS_Relaxed)
	{
		CharacterInventory->UnsheatheEquipment();
	}
	else if(!bAttacking)
	{
		CharacterInventory->SheatheEquipment();
	}
}

// void AMain::IncrementCoins(int32 Amount)
// {
// 	Coins += Amount;
// }

float AMain::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if(Health > 0.0f)
	{
		Health -= DamageAmount;
		if(Health <= 0.0f)
		{
			Die();
			return -1;
		}
		else
		{
			return DamageAmount;
		}
	}
	else
	{
		return -1;
	}

}

void AMain::Die()
{
	bIsAlive = false;

	if(MainAnimInstance)
	{
		MainAnimInstance->PlayDeathAnimation();
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMain::OnDeathNotify()
{
	GetMesh()->bPauseAnims = true;
}
