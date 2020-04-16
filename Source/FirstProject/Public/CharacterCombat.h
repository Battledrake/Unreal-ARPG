// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterCombat.generated.h"

class UBoxComponent;
class AEnemy;
class UParticleSystem;
class USoundCue;
class AWeapon;
class AMain;
class AMainPlayerController;
class UMainAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPROJECT_API UCharacterCombat : public UActorComponent
{
	GENERATED_BODY()

	// Sets default values for this component's properties
	UCharacterCombat();

public:	

	void StartAttack();
	void DamageTarget(AEnemy* Enemy, AWeapon* WeaponInstigator);
	void SetFocusTarget(AEnemy* Target);

	FORCEINLINE UParticleSystem* GetCharacterHitParticles() { return CharacterHitParticles; }
	FORCEINLINE TArray<FName> GetCharacterHitSockets() { return CharacterHitSockets; }
	FORCEINLINE USoundCue* GetCharacterHitSound() { return CharacterHitSound; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Base")
	UBoxComponent* MeleeCollider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Character")
	UParticleSystem* CharacterHitParticles;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Character")
	TArray<FName> CharacterHitSockets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Character")
	USoundCue* CharacterHitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Info")
	AWeapon* AttackingWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Info")
	AEnemy* FocusTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Info")
	float InterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Combat | Info")
	float DistanceToInterp;

	TArray<AEnemy*> AttackHits;
	FTimerHandle FocusTargetHandle;
	AMain* Main;
	AMainPlayerController* MainPC;
	UMainAnimInstance* MainAnim;
	bool bInterpToEnemy;
	bool bIsAttacking;

	UFUNCTION()
	void MeleeColliderOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetFocusTarget();
	void ApplyDamageEffects(AEnemy* Enemy, UParticleSystem * HitParticles, USoundCue * HitSound);
	void SetInterpToEnemy(bool Value) { bInterpToEnemy = Value; }
	int32 CalculateDamage(AWeapon* WeaponInstigator); //TODO Params for enemy info for ac calculations.

	UFUNCTION(BlueprintCallable)
	void ActivatePrimaryAttack();
	UFUNCTION(BlueprintCallable)
	void ActivateSecondaryAttack();
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();
	UFUNCTION(BlueprintCallable)
	void ActivateRangedAttack();
	UFUNCTION(BlueprintCallable)
	void AttackEnded();
	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();
		
};
