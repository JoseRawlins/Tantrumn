// Fill out your copyright notice in the Description page of Project Settings.


#include "TantrumnGameModeBase.h"
#include "TantrumnGameWidget.h"

ATantrumnGameModeBase::ATantrumnGameModeBase()
{

}

void ATantrumnGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameState = EGameState::WAITING;
	DisplayCountdown();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATantrumnGameModeBase::StartGame, GameCountDownDuration, false);
}

EGameState ATantrumnGameModeBase::GetCurrentGameState()
{
	return CurrentGameState;
}

void ATantrumnGameModeBase::PlayerReachedEnd()
{
	CurrentGameState = EGameState::GameOver;
	
	//GameWidget->LevelComplete();
	FInputModeUIOnly InputMode;
	//PC->SetInputMode(InputMode);
	//PC->SetShowMouseCursor(true);
	
}

void ATantrumnGameModeBase::DisplayCountdown()
{
	//if (!GameWidgetClass) { Return; }

	//PC = UGamePlayStatics::GetPlayerController(GetWorld(), 0);
	//GameWidget->AddToViewport();
	//GameWidget->StartCountdown(GameCountdownDuration, this);
}

void ATantrumnGameModeBase::StartGame()
{
	CurrentGameState = EGameState::Playing;
	FInputModeGameOnly InputMode;
	//PC->SetInputMode(InputMode);
	//PC->SetShowMouseCursor(false);
}
