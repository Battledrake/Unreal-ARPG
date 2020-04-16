// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Ammunition.generated.h"

UENUM(BlueprintType)
enum class EAmmunitionType : uint8
{
	Arrow,
	Bolt,
	Bullet
};

class UBoxComponent;
class UProjectileMovementComponent;
class AWeapon;
/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AAmmunition : public AEquipment
{
	GENERATED_BODY()

		AAmmunition();

public:

	void Initialize(AWeapon* WeaponInstigator, AMain* Main);

	FORCEINLINE EAmmunitionType GetAmmoType() { return AmmoType; }

protected:

	void BeginPlay() override;

// 	void Equip(AMain* Main) override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Ammo | Base")
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Ammo | Base")
	UBoxComponent* ArrowCollider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Ammo | Base")
	EAmmunitionType AmmoType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Ammo | Base")
	float ProjectileSpeed;

	AWeapon* WeaponInstigator;
	AMain* Main;

	UFUNCTION()
	void OnArrowCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
