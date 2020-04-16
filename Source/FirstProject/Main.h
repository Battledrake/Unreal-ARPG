// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

class AItem;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class USoundCue;
class UParticleSystem;
class AEnemy;
class AMainPlayerController;
class UMainAnimInstance;
class UInventory;
class UCharacterCombat;
class UBoxComponent;

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

	AMain();

public:

	void SetMovementStatus(EMovementStatus Status);
	void SetIsAttacking(bool Value) { bAttacking = Value; }
	FRotator GetLookAtRotationYaw(FVector Target);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool bGetAttackButtonDown() { return bAttackButtonDown; }
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }
	FORCEINLINE UInventory* GetCharacterInventory() { return CharacterInventory; }
	FORCEINLINE UCharacterCombat* GetCharacterCombat() { return CharacterCombat; }
	FORCEINLINE UBoxComponent* GetMeleeCollider() { return MeleeCollider; }

// 	void IncrementCoins(int32 Amount);

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "Movement")
	float OrientRotationRate;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "Movement")
	float JumpVelocity;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "Movement")
	float AirControl;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	UInventory* CharacterInventory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat")
	UCharacterCombat* CharacterCombat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat")
	UBoxComponent* MeleeCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Enums")
	EMovementStatus MovementStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Enums")
	EStaminaStatus StaminaStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float MaxStamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	int32 Coins;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float MinSprintStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Player | Stats")
	float SprintingSpeed;

	AMainPlayerController* MainPC;
	UMainAnimInstance* MainAnimInstance;
	bool bAttacking;
	bool bShiftKeyDown;
	bool bAttackButtonDown;
	bool bIsAlive;

	UFUNCTION(BlueprintCallable)
	void OnDeathNotify();

	void Die();

	//Input Methods
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Interact();
	void Attack();
	void StopAttacking();
	void MoveWeapon();
	void ShiftKeyDown();
	void ShiftKeyUp();
	void Jump();
};
