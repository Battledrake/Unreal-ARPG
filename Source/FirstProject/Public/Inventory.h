// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

class AItem;
class AEquipment;
class AWeapon;
class AMain;
class UMainAnimInstance;
class AAmmunition;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIRSTPROJECT_API UInventory : public UActorComponent
{
	GENERATED_BODY()

	// Sets default values for this component's properties
	UInventory();

public:

	void EquipToHeadSlot(AEquipment* ItemToEquip);
	void EquipToChestSlot(AEquipment* ItemToEquip);
	void EquipToBeltSlot(AEquipment* ItemToEquip);
	void EquipToLegsSlot(AEquipment* ItemToEquip);
	void EquipToBootsSlot(AEquipment* ItemToEquip);
	void EquipToNeckSlot(AEquipment* ItemToEquip);
	void EquipToGemSlot(AEquipment* ItemToEquip);
	void EquipToMainHandSlot(AWeapon* ItemToEquip);
	void EquipToOffHandSlot(AEquipment* ItemToEquip);
	void EquipToRingSlot1(AEquipment* ItemToEquip);
	void EquipToRingSlot2(AEquipment* ItemToEquip);
	void EquipWeapon(AWeapon* WeaponToEquip, uint8 WeaponType);

// 	void SetArrowSlot(AAmmunition* Ammo) { ArrowSlot = Ammo; }

	void SheatheEquipment();
	void UnsheatheEquipment();

	FORCEINLINE bool bGetIsSheathed() { return bIsSheathed; }
	FORCEINLINE AWeapon* GetPrimaryWeapon() { return PrimaryWeapon; }
	FORCEINLINE AWeapon* GetSecondaryWeapon() { return SecondaryWeapon; }
	FORCEINLINE TSubclassOf<AAmmunition> GetArrowSlot() { return ArrowSlot; }

protected:

	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	AEquipment* HeadSlot;
	AEquipment* ChestSlot;
	AEquipment* BeltSlot;
	AEquipment* LegsSlot;
	AEquipment* BootsSlot;
	AEquipment* NeckSlot;
	AEquipment* GemSlot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory | EquipmentSlots")
	AWeapon* MainHandSlot1;
	AEquipment* OffHandSlot1;
	AWeapon* MainHandSlot2;
	AEquipment* OffHandSlot2;
	AEquipment* RingSlot1;
	AEquipment* RingSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	TArray<AItem*> InventoryArray;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory | EquipmentSlots")
	TSubclassOf<AAmmunition> ArrowSlot;

	AMain* Main;
	UMainAnimInstance* MainAnim;
	AWeapon* PrimaryWeapon;
	AWeapon* SecondaryWeapon;
	bool bIsSheathed;

	FName HeadSocket = FName("head_accessories");
	FName MainHand = FName("RightHand");
	FName OffHand = FName("LeftHand");
	FName BowHand = FName("BowHand");
	FName Shield = FName("ShieldArm");

	void AttachEquipment(AEquipment* EquipmentToAttach, FName Socket = NAME_None);
	void RemoveEquipment(AEquipment* EquipmentInSlot);
};
