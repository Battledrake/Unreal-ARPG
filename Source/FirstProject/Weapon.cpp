// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Inventory.h"

AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("Mesh")))
{

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionCOmponent"));
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetupAttachment(GetRootComponent());

	bWeaponParticle = false;

	WeaponType = EWeaponType::EWT_MAX;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	EquipmentType = EEquipmentType::EET_Weapon;

	if(WeaponType != EWeaponType::EWT_MainHand &&
		WeaponType != EWeaponType::EWT_OneHand)
		bIsTwoHander = true;

	if (!bWeaponParticle)	IdleParticlesComponent->Deactivate();
}

void AWeapon::Equip(AMain* Owner)
{
	if (Owner)
	{
		UInventory* Inventory = Owner->GetCharacterInventory();
		Inventory->EquipWeapon(this, (int)WeaponType);
	}

	//TODO Better modularity to allow removal of owner and addition of multiple types.
}
