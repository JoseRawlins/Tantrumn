// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TantrumnCharacterBase.generated.h"

UCLASS()
class TANTRUMN_API ATantrumnCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATantrumnCharacterBase();

	virtual void RequestSprintStart();
	virtual void RequestSprintEnd();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MinImpactSpeed = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MaxImpactSpeed = 1600.0f;

	//Time in Seconds
	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MinStunTime = 1.0f;
	//Time in Seconds
	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MaxStunTime = 1.0f;

	float StunTime = 0.0f;
	float StunBeginTimeStamp = 0.0f;

	bool bIsStunned = false;
	bool bIsSprinting = false;

	float MaxWalkSpeed = 0.0f;

	void OnStunBegin(float StunRatio);
	void OnStunEnd();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

};
