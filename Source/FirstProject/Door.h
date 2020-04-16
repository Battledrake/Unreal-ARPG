// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class FIRSTPROJECT_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* DoorMesh;

	UPROPERTY(BlueprintReadWrite, Category = "Door")
	FVector InitialDoorLocation;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ActivateDoor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DeactivateDoor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OpenDoor();
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void UpdateDoorLocation(float ZValue);

private:

	FTimerHandle DoorHandle;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = true))
	float DoorOpenDelay;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = true))
	float DoorCloseDelay;
};
