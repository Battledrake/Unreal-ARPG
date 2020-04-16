// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "Equipment.h"
#include "Weapon.h"
#include "Main.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MainAnimInstance.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Main = Cast<AMain>(this->GetOwner());
	if(Main)
	{
		MainAnim = Cast<UMainAnimInstance>(Main->GetMesh()->GetAnimInstance());
	}

}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventory::AttachEquipment(AEquipment* EquipmentToAttach, FName Socket)
{

	EquipmentToAttach->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
	EquipmentToAttach->GetSkeletalMesh()->SetRelativeLocation(FVector::ZeroVector);
	EquipmentToAttach->GetSkeletalMesh()->SetMasterPoseComponent(Main->GetMesh());
	EquipmentToAttach->GetSkeletalMesh()->bUseBoundsFromMasterPoseComponent = true;
	EquipmentToAttach->DisableCollisionVolume();
}

void UInventory::EquipWeapon(AWeapon * WeaponToEquip, uint8 WeaponType)
{
	WeaponToEquip->DisableCollisionVolume();
	if(WeaponType == (int)EWeaponType::EWT_MainHand || WeaponToEquip->bGetIsTwoHander() || MainHandSlot1 == nullptr)
	{
		EquipToMainHandSlot(WeaponToEquip);
	}
	else if(MainHandSlot1 && WeaponType == (int)EWeaponType::EWT_OneHand)
	{
		EquipToOffHandSlot(WeaponToEquip);
	}
}

void UInventory::RemoveEquipment(AEquipment * EquipmentInSlot)
{
	EquipmentInSlot->SetActorHiddenInGame(true);
	InventoryArray.Add(EquipmentInSlot);
}

void UInventory::EquipToHeadSlot(AEquipment* ItemToEquip)
{
	if(HeadSlot)
	{
		RemoveEquipment(HeadSlot);
	}
	HeadSlot = ItemToEquip;
	AttachEquipment(HeadSlot, HeadSocket);
	//TODO SpawnActor on Character
}

void UInventory::EquipToChestSlot(AEquipment * ItemToEquip)
{
	if(ChestSlot)
	{
		RemoveEquipment(ChestSlot);
	}
	ChestSlot = ItemToEquip;
	AttachEquipment(ChestSlot);
	//TODO SpawnActor on Character
}

void UInventory::EquipToBeltSlot(AEquipment * ItemToEquip)
{
}

void UInventory::EquipToLegsSlot(AEquipment * ItemToEquip)
{
	if(LegsSlot)
	{
		RemoveEquipment(LegsSlot);
	}
	LegsSlot = ItemToEquip;
	AttachEquipment(LegsSlot);
}

void UInventory::EquipToBootsSlot(AEquipment * ItemToEquip)
{
	if(BootsSlot)
	{
		RemoveEquipment(BootsSlot);
	}
	BootsSlot = ItemToEquip;
	AttachEquipment(BootsSlot);
}

void UInventory::EquipToNeckSlot(AEquipment * ItemToEquip)
{
	if(NeckSlot)
	{
		RemoveEquipment(NeckSlot);
	}
	NeckSlot = ItemToEquip;
	AttachEquipment(NeckSlot);
}

void UInventory::EquipToGemSlot(AEquipment * ItemToEquip)
{
}

void UInventory::EquipToMainHandSlot(AWeapon* WeaponToEquip)
{
	if(WeaponToEquip->bGetIsTwoHander())
	{
		if(MainHandSlot1)
		{
			RemoveEquipment(MainHandSlot1);
			MainHandSlot1->OnDeactivateEquipment();
			MainHandSlot1 = WeaponToEquip;
		}
		if(OffHandSlot1)
		{
			RemoveEquipment(OffHandSlot1);
			OffHandSlot1->OnDeactivateEquipment();
			OffHandSlot1 = nullptr;
			SecondaryWeapon = nullptr;

		}
		switch(WeaponToEquip->GetWeaponType())
		{
			case EWeaponType::EWT_TwoHandSword:
				MainAnim->SetCombatState(EAnimationState::EAS_2HSword);
				break;
			case EWeaponType::EWT_Bow:
				MainAnim->SetCombatState(EAnimationState::EAS_Bow);
				break;
			case EWeaponType::EWT_Fists:
				MainAnim->SetCombatState(EAnimationState::EAS_Unarmed);
				break;
		}
	}
	else
	{
		if(MainHandSlot1)
		{
			RemoveEquipment(MainHandSlot1);
			MainHandSlot1->OnDeactivateEquipment();
		}
		if(OffHandSlot1)
		{
			if(OffHandSlot1->GetEquipmentType() == EEquipmentType::EET_Weapon)
				MainAnim->SetCombatState(EAnimationState::EAS_Dual);
			else
				MainAnim->SetCombatState(EAnimationState::EAS_Armed);
		}
		else
		{
			MainAnim->SetCombatState(EAnimationState::EAS_Armed);
		}

	}
	if(WeaponToEquip->GetWeaponType() != EWeaponType::EWT_Bow)
		WeaponToEquip->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MainHand);
	else
		WeaponToEquip->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, BowHand);
	WeaponToEquip->GetSkeletalMesh()->SetRelativeLocation(FVector::ZeroVector);
	MainHandSlot1 = WeaponToEquip;
	MainHandSlot1->OnActivateEquipment();
	PrimaryWeapon = MainHandSlot1;
	UnsheatheEquipment();
}

void UInventory::EquipToOffHandSlot(AEquipment * ItemToEquip)
{
	if(ItemToEquip->GetEquipmentType() == EEquipmentType::EET_Shield)
	{
		if(MainHandSlot1)
		{
			if(MainHandSlot1->bGetIsTwoHander())
			{
				RemoveEquipment(MainHandSlot1);
				MainHandSlot1->OnDeactivateEquipment();
				MainHandSlot1 = nullptr;
				PrimaryWeapon = nullptr;
			}
		}
		if(OffHandSlot1)
		{
			RemoveEquipment(OffHandSlot1);
			OffHandSlot1->OnDeactivateEquipment();
			SecondaryWeapon = nullptr;
		}
		ItemToEquip->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Shield);
		OffHandSlot1 = ItemToEquip;
		OffHandSlot1->OnActivateEquipment();
		MainAnim->SetCombatState(EAnimationState::EAS_Armed);
		UnsheatheEquipment();
	}
	else if(ItemToEquip->GetEquipmentType() == EEquipmentType::EET_Weapon)
	{
		AWeapon* Weapon = Cast<AWeapon>(ItemToEquip);
		if(MainHandSlot1 == nullptr)
		{
			EquipToMainHandSlot(Weapon);
		}
		else if(MainHandSlot1)
		{
			if(MainHandSlot1->bGetIsTwoHander())
			{
				RemoveEquipment(MainHandSlot1);
				MainHandSlot1->OnDeactivateEquipment();
				EquipToMainHandSlot(Weapon);
			}
			else
			{
				if(OffHandSlot1)
				{
					RemoveEquipment(OffHandSlot1);
					OffHandSlot1->OnDeactivateEquipment();
				}
				ItemToEquip->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, OffHand);
				ItemToEquip->GetSkeletalMesh()->SetRelativeLocation(FVector::ZeroVector);
				OffHandSlot1 = ItemToEquip;
				OffHandSlot1->OnActivateEquipment();
				SecondaryWeapon = Cast<AWeapon>(OffHandSlot1);
				MainAnim->SetCombatState(EAnimationState::EAS_Dual);
				UnsheatheEquipment();
			}
		}
	}
}

void UInventory::EquipToRingSlot1(AEquipment * ItemToEquip)
{
}

void UInventory::EquipToRingSlot2(AEquipment * ItemToEquip)
{
}

void UInventory::SheatheEquipment()
{
	bIsSheathed = true;
	if(MainHandSlot1)
	{
		MainHandSlot1->IdleParticlesComponent->Deactivate();
		MainHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MainHandSlot1->GetPrimaryMountLoc());
		MainHandSlot1->OnDeactivateEquipment();
	}
	if(OffHandSlot1)
	{
		OffHandSlot1->IdleParticlesComponent->Deactivate();
		if(OffHandSlot1->GetEquipmentType() == EEquipmentType::EET_Shield)
			OffHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, OffHandSlot1->GetPrimaryMountLoc());
		else
			OffHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, OffHandSlot1->GetSecondaryMountLoc());
		OffHandSlot1->OnDeactivateEquipment();
	}
	MainAnim->ActivateRelaxedState();
}

void UInventory::UnsheatheEquipment()
{
	bIsSheathed = false;
	if(OffHandSlot1)
	{
		OffHandSlot1->IdleParticlesComponent->Activate();
		if(OffHandSlot1->GetEquipmentType() == EEquipmentType::EET_Shield)
		{
			OffHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Shield);
		}
		else
		{
			OffHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, OffHand);
		}
		OffHandSlot1->OnActivateEquipment();
	}
	if(MainHandSlot1)
	{
		MainHandSlot1->IdleParticlesComponent->Activate();
		if(MainHandSlot1->GetWeaponType() != EWeaponType::EWT_Bow)
			MainHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MainHand);
		else
			MainHandSlot1->AttachToComponent(Main->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, BowHand);
		MainHandSlot1->OnActivateEquipment();
	}
	MainAnim->ActivateCombatState();
}

