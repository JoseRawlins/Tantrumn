// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TantrumnGameModeBase.generated.h"


UENUM(BlueprintType)
enum class EGameState :uint8
{
	NONE		 UMETA(DisplayName = "Waiting"),
	WAITING		 UMETA(DisplayName = "Waiting"),
	Playing		 UMETA(DisplayName = "Playing"),
	Paused		 UMETA(DisplayName = "Paused"),
	GameOver     UMETA(DisplayName = "GameOver"),
};

UCLASS()
class TANTRUMN_API ATantrumnGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public: 

	// Functions
	ATantrumnGameModeBase();

	virtual void BeginPlay() override;

	EGameState GetCurrentGameState();
	void PlayerReachedEnd();

private:

	// vars

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="States", meta = (AllowPrivateAccess = "True"))
	EGameState CurrentGameState = EGameState::NONE;
	//
	UPROPERTY(EditAnywhere, Category ="Game Details")
	float GameCountDownDuration = 4.0f;

	FTimerHandle TimerHandle;

	//TODo

	// FUNCTIONS

	void DisplayCountdown();
	void StartGame();

};
