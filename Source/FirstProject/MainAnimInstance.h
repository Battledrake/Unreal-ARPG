// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	EAS_Relaxed		UMETA(DisplayName = "Relaxed"),
	EAS_Unarmed		UMETA(DisplayName = "Unarmed"),
	EAS_Armed		UMETA(DisplayName = "Armed"),
	EAS_Dual		UMETA(DisplayName = "Dual"),
	EAS_2HSword		UMETA(DisplayName = "2HSword"),
	EAS_2HAxe		UMETA(DisplayName = "2HAxe"),
	EAS_2HSpear		UMETA(DisplayName = "2HSpear"),
	EAS_Bow			UMETA(DisplayName = "Bow"),
	EAS_Crossbow	UMETA(DisplayName = "Crossbow"),
};

class UAnimMontage;

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "AnimationProperties")
	void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	class APawn* Pawn;
	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "Movement")
	class AMain* Main;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	void PlayAttackAnimation();
	void PlayDeathAnimation();


	FORCEINLINE EAnimationState GetAnimationState() { return CurrentAnimationState; }
	void SetActiveMontage(EAnimationState AnimationType) { ActiveMontage = CombatMontages[(int)AnimationType]; }
	void SetCombatState(EAnimationState NewState) { CurrentAnimationState = NewState; SavedCombatState = NewState; SetActiveMontage(NewState); }
	void ActivateRelaxedState() { CurrentAnimationState = RelaxedState; }
	void ActivateCombatState() { CurrentAnimationState = SavedCombatState; }

private:

	//Order(Unarmed, Armed, Dual, 2hSword, 2HAxe, 2HSpear, Bow, Crossbow)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Animation | Base")
	TArray<UAnimMontage*> CombatMontages;

	UAnimMontage* ActiveMontage;
	int32 MontageIncrementor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Animation | States")
	EAnimationState CurrentAnimationState;
	EAnimationState SavedCombatState;
	EAnimationState RelaxedState = EAnimationState::EAS_Relaxed;
};
