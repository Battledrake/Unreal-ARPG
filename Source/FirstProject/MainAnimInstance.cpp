// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"
#include "Inventory.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if(Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if(Pawn)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if(Pawn == nullptr)
		Pawn = TryGetPawnOwner();

	if(Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if(Main == nullptr)
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

void UMainAnimInstance::PlayAttackAnimation()
{
	switch(MontageIncrementor)
	{
		case 0:
			//TODO Setup a variable in weapon that determines animation speed
			MontageIncrementor++;
			Montage_Play(ActiveMontage, 1.0f);
			Montage_JumpToSection(FName("Attack_1"));
			break;
		case 1:
			MontageIncrementor++;
			Montage_Play(ActiveMontage, 1.0f);
			Montage_JumpToSection(FName("Attack_2"));
			break;
		case 2:
			MontageIncrementor++;
			Montage_Play(ActiveMontage, 1.0f);
			Montage_JumpToSection(FName("Attack_3"));
			break;
	}
	if(MontageIncrementor >= 3)
		MontageIncrementor = 0;
}

void UMainAnimInstance::PlayDeathAnimation()
{
	Montage_Play(ActiveMontage);
	Montage_JumpToSection(FName("Death"));
}

