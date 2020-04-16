// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Equipment.generated.h"

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EET_Head		UMETA(DisplayName = "Head"),
	EET_Chest		UMETA(DisplayName = "Chest"),
	EET_Belt		UMETA(DisplayName = "Belt"),
	EET_Legs		UMETA(DisplayName = "Legs"),
	EET_Boots		UMETA(DisplayName = "Boots"),
	EET_Neck		UMETA(DisplayName = "Neck"),
	EET_Gem			UMETA(DisplayName = "Gem"),
	EET_Weapon		UMETA(DisplayName = "Weapon"),
	EET_Shield		UMETA(DisplayName = "Shield"),
	EET_Ring		UMETA(DisplayName = "Ring")
};

class USkeletalMeshComponent;
class AMain;

UCLASS()
class FIRSTPROJECT_API AEquipment : public AItem
{
	GENERATED_BODY()

public:

	AEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintImplementableEvent)
	void OnActivateEquipment();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeactivateEquipment();

	FORCEINLINE EEquipmentType GetEquipmentType() { return EquipmentType; }
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMesh() { return SkeletalMesh; }
	FORCEINLINE FName GetPrimaryMountLoc() { return PrimaryMountLoc; }
	FORCEINLINE FName GetSecondaryMountLoc() { return SecondaryMountLoc; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Equipment | Base")
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Equipment | Base")
	EEquipmentType EquipmentType;

	virtual void Equip(AMain* Main);

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Equipment | Base")
	FName PrimaryMountLoc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Equipment | Base")
	FName SecondaryMountLoc;
};
