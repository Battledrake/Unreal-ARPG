// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Main.h"
#include "Inventory.h"

AEquipment::AEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("Mesh")))
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());
}

void AEquipment::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			Equip(Main);
		}
	}
}

void AEquipment::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void AEquipment::Equip(AMain* Main)
{
	UInventory* Inventory = Main->GetCharacterInventory();
	if(Inventory)
	{
		switch(EquipmentType)
		{
			case EEquipmentType::EET_Head:
				Inventory->EquipToHeadSlot(this);
				break;
			case EEquipmentType::EET_Chest:
				Inventory->EquipToChestSlot(this);
				break;
			case EEquipmentType::EET_Belt:
				Inventory->EquipToBeltSlot(this);
				break;
			case EEquipmentType::EET_Legs:
				Inventory->EquipToLegsSlot(this);
				break;
			case EEquipmentType::EET_Boots:
				Inventory->EquipToBootsSlot(this);
				break;
			case EEquipmentType::EET_Neck:
				Inventory->EquipToNeckSlot(this);
				break;
			case EEquipmentType::EET_Shield:
				Inventory->EquipToOffHandSlot(this);
				break;
		}
	}
}

