// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
ADoor::ADoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));

	DoorOpenDelay = 0.5f;
	DoorCloseDelay = 2.0f;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	InitialDoorLocation = GetActorLocation();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoor::ActivateDoor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GetWorldTimerManager().SetTimer(DoorHandle, this, &ADoor::OpenDoor, DoorOpenDelay);
}

void ADoor::DeactivateDoor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().SetTimer(DoorHandle, this, &ADoor::CloseDoor, DoorCloseDelay);
}

void ADoor::UpdateDoorLocation(float ZValue)
{
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += ZValue;
	SetActorLocation(NewLocation);
}