// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "CharacterCombat.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphere"));
	AggroSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AggroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AggroSphere->InitSphereRadius(1500.0f);
	AggroSphere->SetupAttachment(GetRootComponent());

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->InitSphereRadius(200.0f);
	CombatSphere->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatBox"));
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CombatCollision->SetupAttachment(GetMesh(), AttackSocket);

	MaxHealth = 100.0f;
	MinDamageDie = 1;
	MaxDamageDie = 6;

	InterpSpeed = 5.0f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	DeathDelay = 3.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroSphereOnOverlapBegin);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AggroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnAttackCollisionEnd);

	Health = MaxHealth;
	bIsAttacking = false;
	bInterpToTarget = false;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bInterpToTarget && CurrentTarget)
	{
		FRotator LookAtRotation = LookAtRotationYaw(CurrentTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
	}

	if(CurrentTarget && EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking && AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		MoveToTarget(CurrentTarget);
	}

}

FRotator AEnemy::LookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtYaw;
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

void AEnemy::AggroSphereOnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			CurrentTarget = Main;
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AggroSphereOnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			CurrentTarget = nullptr;
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			AIController->StopMovement();
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			bInterpToTarget = true;
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			AIController->StopMovement();
			if(GetWorldTimerManager().IsTimerPaused(AttackTimer))
			{
				GetWorldTimerManager().UnPauseTimer(AttackTimer);
			}
			else
			{
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			GetWorldTimerManager().PauseTimer(AttackTimer);
			bInterpToTarget = false;
			EnemyMovementStatus = EEnemyMovementStatus::EMS_MoveToTarget;
			if(!bIsAttacking)
			{
				MoveToTarget(CurrentTarget);
			}

		}
	}
}

void AEnemy::OnAttackCollisionBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherComp)
	{
		if(OtherComp->GetFName() == FName("CollisionCylinder"))
		{
			if(OtherActor)
			{
				AMain* Main = Cast<AMain>(OtherActor);
				if(Main)
				{
					if(!HitArray.Contains(Main))
					{
						HitArray.Add(Main);
						int32 DamageRolled = FMath::RandRange(MinDamageDie, MaxDamageDie);
						if(DamageTypeClass)
						{
							float DamageDealt = UGameplayStatics::ApplyDamage(Main, DamageRolled, AIController, this, DamageTypeClass);
							if(DamageDealt == -1)
							{
								CurrentTarget = nullptr;
								SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
							}
						}
						ApplyDamageEffects(Main);
					}
				}
			}
		}
	}
}

void AEnemy::ApplyDamageEffects(AMain* Main)
{
	if(Main->GetCharacterCombat()->GetCharacterHitParticles())
	{
		if(Main->GetCharacterCombat()->GetCharacterHitSockets().Num() > 0)
		{
			int32 Random = FMath::RandRange(0, Main->GetCharacterCombat()->GetCharacterHitSockets().Num() - 1);
			const USkeletalMeshSocket* HitSocket = Main->GetMesh()->GetSocketByName(Main->GetCharacterCombat()->GetCharacterHitSockets()[Random]);
			if(HitSocket)
			{
				FVector SocketLocation = HitSocket->GetSocketLocation(Main->GetMesh());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->GetCharacterCombat()->GetCharacterHitParticles(), SocketLocation);
				if(Main->GetCharacterCombat()->GetCharacterHitSound())
				{
					UGameplayStatics::PlaySound2D(this, Main->GetCharacterCombat()->GetCharacterHitSound());
				}
			}
		}
	}
}

void AEnemy::OnAttackCollisionEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::Attack()
{
	if(!bIsAttacking)
	{
		bIsAttacking = true;
		bInterpToTarget = false;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
		}
	}
}

void AEnemy::PlayAttackSound()
{
	if(AttackSound)
	{
		UGameplayStatics::PlaySound2D(this, AttackSound);
	}
}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitArray.Empty();
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnAttackEnd()
{
	bIsAttacking = false;

	if(CurrentTarget)
	{
		if(EnemyMovementStatus == EEnemyMovementStatus::EMS_MoveToTarget)
		{
			MoveToTarget(CurrentTarget);
		}
		else
		{
			bInterpToTarget = true;

			float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		}
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if(AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(50.0f);

		AIController->MoveTo(MoveRequest);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	OnTakeDamage(DamageAmount);

	if(Health > 0.0f)
	{
		Health -= DamageAmount;

		CurrentTarget = EventInstigator;
		MoveToTarget(EventInstigator);

		if(Health <= 0.0f)
		{
			Die();
			return -1;
		}
		else
		{
			return DamageAmount;
		}
	}
	else
	{
		return -1;
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	UGameplayStatics::PlaySound2D(this, DeathSound);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnDeathNotify()
{
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

void AEnemy::Disappear()
{
	Destroy();
}
