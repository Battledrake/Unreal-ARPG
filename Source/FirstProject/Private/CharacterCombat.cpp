// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCombat.h"
#include "Components/BoxComponent.h"
#include "Main.h"
#include "Inventory.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MainPlayerController.h"
#include "Main.h"
#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MainAnimInstance.h"
#include "TimerManager.h"
#include "Ammunition.h"

// Sets default values for this component's properties
UCharacterCombat::UCharacterCombat()
{
	PrimaryComponentTick.bCanEverTick = true;

	InterpSpeed = 5.0f;
	DistanceToInterp = 300.0f;
	bInterpToEnemy = false;
}

// Called when the game starts
void UCharacterCombat::BeginPlay()
{
	Super::BeginPlay();

	Main = Cast<AMain>(GetOwner());

	UAnimInstance* AnimInstance = Main->GetMesh()->GetAnimInstance();
	MainAnim = Cast<UMainAnimInstance>(AnimInstance);

	MainPC = Cast<AMainPlayerController>(Main->GetController());

	MeleeCollider = Main->GetMeleeCollider();
	MeleeCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeleeCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	MeleeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeleeCollider->OnComponentBeginOverlap.AddDynamic(this, &UCharacterCombat::MeleeColliderOnOverlap);
}


// Called every frame
void UCharacterCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector LineStart = Main->GetActorLocation();
	FVector LineEnd = LineStart + Main->GetActorForwardVector() * 1000.0f;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Main);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	FHitResult HitInfo;
	if(GetWorld()->LineTraceSingleByObjectType(HitInfo, LineStart, LineEnd, ObjectParams, TraceParams))
	{
		AEnemy* Enemy = Cast<AEnemy>(HitInfo.GetActor());
		if(Enemy)
		{
			SetFocusTarget(Enemy);
		}
	}

	if(bInterpToEnemy && FocusTarget)
	{
		FRotator LookAtYaw = Main->GetLookAtRotationYaw(FocusTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(Main->GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		Main->SetActorRotation(InterpRotation);
	}
}

void UCharacterCombat::StartAttack()
{
	// 	bool bHasWeapon = Main->GetCharacterInventory()->GetPrimaryWeapon();

	if(!bIsAttacking)
	{
		Main->SetMovementStatus(EMovementStatus::EMS_Normal);

		if(Main->GetCharacterInventory()->bGetIsSheathed())
			Main->GetCharacterInventory()->UnsheatheEquipment();

		bIsAttacking = true;
		Main->SetIsAttacking(bIsAttacking);

		if(FocusTarget)
		{
			FVector DistanceToEnemy = FocusTarget->GetActorLocation() - Main->GetActorLocation();
			if(DistanceToEnemy.Size() < DistanceToInterp)
			{
				SetInterpToEnemy(true);
			}
		}

		if(MainAnim)
		{
			MainAnim->PlayAttackAnimation();
		}
	}
}

void UCharacterCombat::ActivatePrimaryAttack()
{
	AttackingWeapon = Main->GetCharacterInventory()->GetPrimaryWeapon();
	MeleeCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UCharacterCombat::ActivateSecondaryAttack()
{
	AttackingWeapon = Main->GetCharacterInventory()->GetSecondaryWeapon();
	MeleeCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UCharacterCombat::DeactivateCollision()
{
	MeleeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackingWeapon = nullptr;
	AttackHits.Empty();
}

void UCharacterCombat::MeleeColliderOnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
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
					if(!AttackHits.Contains(Enemy))
					{
						AttackHits.Add(Enemy);
						if(AttackingWeapon->GetDamageTypeClass())
						{
							DamageTarget(Enemy, AttackingWeapon);
						}
					}
				}
			}
		}
	}
}

void UCharacterCombat::DamageTarget(AEnemy* Enemy, AWeapon* WeaponInstigator)
{
	int32 DamageRolled = CalculateDamage(WeaponInstigator);
	UGameplayStatics::ApplyDamage(Enemy, DamageRolled, MainPC, WeaponInstigator, WeaponInstigator->GetDamageTypeClass());

	ApplyDamageEffects(Enemy, Enemy->GetHitParticles(), Enemy->GetHitSound());
}

void UCharacterCombat::ApplyDamageEffects(AEnemy* Enemy, UParticleSystem * HitParticles, USoundCue * HitSound)
{
	if(AttackingWeapon->GetImpactSound())
	{
		UGameplayStatics::PlaySound2D(this, AttackingWeapon->GetImpactSound());
	}

	if(Enemy->GetHitParticles())
	{
		if(Enemy->GetHitSockets().Num() > 0)
		{
			int32 Random = FMath::RandRange(0, Enemy->GetHitSockets().Num() - 1);
			const USkeletalMeshSocket* HitSocket = Enemy->GetMesh()->GetSocketByName(Enemy->GetHitSockets()[Random]);
			if(HitSocket)
			{
				FVector SocketLocation = HitSocket->GetSocketLocation(Enemy->GetMesh());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->GetHitParticles(), SocketLocation);
				if(Enemy->GetHitSound())
				{
					UGameplayStatics::PlaySound2D(this, Enemy->GetHitSound());
				}
			}
		}
	}
}

int32 UCharacterCombat::CalculateDamage(AWeapon* WeaponInstigator)
{
	//TODO Implement AB/AC calculations for Criticals.
	int32 TotalDamage = 0;
	for(int i = 0; i < WeaponInstigator->GetNumDice(); i++)
	{
		TotalDamage += FMath::RandRange(1, WeaponInstigator->GetDieToRoll());
	}
	return TotalDamage;
}

void UCharacterCombat::SetFocusTarget()
{
	FocusTarget = nullptr;

	MainPC->HideEnemyHealthBar();
}
void UCharacterCombat::SetFocusTarget(AEnemy* Target)
{
	FocusTarget = Target;

	Main->GetWorldTimerManager().SetTimer(FocusTargetHandle, this, &UCharacterCombat::SetFocusTarget, 2.0f);

	MainPC->DisplayEnemyHealthBar();
}

void UCharacterCombat::ActivateRangedAttack()
{
	if(Main->GetCharacterInventory()->GetArrowSlot())
	{
// 		AAmmunition* Projectile = GetWorld()->SpawnActor<AAmmunition>(Main->GetActorLocation(), Main->GetActorRotation());
		FVector SpawnLoc = Main->GetActorLocation();
		FRotator SpawnRot = Main->GetActorRotation();
		TSubclassOf<AAmmunition> SpawnClass = Main->GetCharacterInventory()->GetArrowSlot();
		AAmmunition* Projectile = GetWorld()->SpawnActor<AAmmunition>(SpawnClass, SpawnLoc, SpawnRot);
		AttackingWeapon = Main->GetCharacterInventory()->GetPrimaryWeapon();
		Projectile->Initialize(AttackingWeapon, Main);
	}
}

void UCharacterCombat::AttackEnded()
{
	bIsAttacking = false;
	Main->SetIsAttacking(bIsAttacking);
	SetInterpToEnemy(false);

	if(Main->bGetAttackButtonDown())
	{
		StartAttack();
	}
}

void UCharacterCombat::PlaySwingSound()
{
	if(AttackingWeapon->GetSwingSound())
	{
		UGameplayStatics::PlaySound2D(this, AttackingWeapon->GetSwingSound());
	}
}

