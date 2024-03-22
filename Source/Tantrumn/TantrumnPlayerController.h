// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundCue.h"
#include "TantrumnPlayerController.generated.h"

class ATantrumnGameModeBase;
class UUserWidget;


UCLASS()
class TANTRUMN_API ATantrumnPlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;

protected:

	void SetupInputComponent() override;

	void RequestMoveForward(float AxisValue);
	void RequestMoveRight(float AxisValue);
	void RequestLookUp(float AxisValue);
	void RequestLookRight(float AxisValue);
	void RequestThrowObject(float AxisValue);

	void RequestPullObject();
	void RequestStopPullObject();

	void RequestJump();
	void RequestStopJump();
	
	void RequestCrouchStart();
	void RequestCrouchEnd();

	void RequestSprintStart();
	void RequestSprintEnd();

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* HUDWidget;

	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookUpRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookRightRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* JumpSound = nullptr;

	ATantrumnGameModeBase* GameModeRef;

	float LastAxis = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Input")
	float FlickThreshold = 0.75f;

	//UPROPERTY(EditAnywhere, Category = "Movement")
	//float SprintSpeed = 1200.0f;

};
