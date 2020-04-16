// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammunition.h"
#include "Components/BoxComponent.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Main.h"
#include "CharacterCombat.h"
#include "Inventory.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

AAmmunition::AAmmunition()
{
	CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ArrowCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ArrowCollider"));
	ArrowCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ArrowCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArrowCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ArrowCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowCollider->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileSpeed = 10000.0f;
}

void AAmmunition::BeginPlay()
{
	Super::BeginPlay();

	ArrowCollider->OnComponentBeginOverlap.AddDynamic(this, &AAmmunition::OnArrowCollision);
}

// void AAmmunition::Equip(AMain * Owner)
// {
// // 	if(Owner)
// // 	{
// // 		UInventory* CharacterInventory = Owner->GetCharacterInventory();
// // 		CharacterInventory->SetArrowSlot(this);
// // 		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// // 		Main = Owner;
// // 		SkeletalMesh->DestroyComponent();
// // 	}
// }

void AAmmunition::Initialize(AWeapon * WeaponThatFiredProjectile, AMain* Main)
{
	WeaponInstigator = WeaponThatFiredProjectile;
	this->Main = Main;
	ProjectileMovement->SetVelocityInLocalSpace(FVector(ProjectileSpeed, 0.0f, 0.0f));
	ArrowCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AAmmunition::OnArrowCollision(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherComp)
	{
		if(OtherComp->GetFName() == FName("CollisionCylinder"))
		{
			if(OtherActor)
			{
				AEnemy* Enemy = Cast<AEnemy>(OtherActor);
				if(Enemy)
				{
					Main->GetCharacterCombat()->DamageTarget(Enemy, WeaponInstigator);
					Main->GetCharacterCombat()->SetFocusTarget(Enemy);
					Destroy();
				}
			}
		}
	}
}
