// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_MainHand		UMETA(DisplayName = "MainHand"),
	EWT_OneHand			UMETA(DisplayName = "OneHand"),
	EWT_Fists			UMETA(DisplayName = "Fist"),
	EWT_TwoHandSword	UMETA(DisplayName = "TwoHandSword"),
	EWT_TwoHandAxe		UMETA(DisplayName = "TwoHandAxe"),
	EWT_TwoHandSpear	UMETA(DisplayName = "TwoHandSpear"),
	EWT_Bow				UMETA(DisplayName = "Bow"),
	EWT_Crossbow		UMETA(DisplayName = "Crossbow"),

	EWT_MAX			UMETA(DisplayName = "DefaultMAX")
};

class UBoxComponent;
class USoundCue;
class AEnemy;
class AMain;

/**
 *
 */
UCLASS()
class FIRSTPROJECT_API AWeapon : public AEquipment
{
	GENERATED_BODY()

public:

	AWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Base")
	class UCapsuleComponent* PickupCollisionComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Effects")
	bool bWeaponParticle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Stats")
	EWeaponType WeaponType;

	FORCEINLINE int32 GetNumDice() { return NumDice; }
	FORCEINLINE int32 GetDieToRoll() { return DieToRoll; }
	FORCEINLINE int32 GetCritThreat() { return CritThreat; }
	FORCEINLINE int32 GetCritHitMult() { return CritHitMult; }

	FORCEINLINE const EWeaponType GetWeaponType() { return WeaponType; }
	FORCEINLINE USoundCue* GetSwingSound() { return SwingSound; }
	FORCEINLINE USoundCue* GetOnEquipSound() { return OnEquipSound; }
	FORCEINLINE USoundCue* GetSheatheSound() { return SheatheSound; }
	FORCEINLINE USoundCue* GetImpactSound() { return ImpactSound; }
	FORCEINLINE bool bGetIsTwoHander() { return bIsTwoHander; }
	FORCEINLINE TSubclassOf<UDamageType> GetDamageTypeClass() { return DamageTypeClass; }

protected:

	virtual void BeginPlay() override;

	void Equip(AMain* Main) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Base")
	UBoxComponent* CombatCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Stats")
	int32 NumDice; //Number of dice to roll
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Stats")
	int32 DieToRoll; //Dice type - d4/d6/d8 etc..
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Stats")
	int32 CritThreat; //The chance to threaten a critical. EX: 2 = 2 chances out of 20, IE 19-20.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Stats")
	int32 CritHitMult; //The critical multiplier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Stats")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Effects")
	USoundCue* OnEquipSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Effects")
	USoundCue* SwingSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Effects")
	USoundCue* ImpactSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon | Effects")
	USoundCue* SheatheSound;

	TArray<AEnemy*> HitArray;
	bool bIsTwoHander;
};
