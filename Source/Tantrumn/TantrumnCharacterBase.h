// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "TantrumnCharacterBase.generated.h"


class AThrowableActor;

UENUM(BlueprintType)
enum class ECharacterThrowState : uint8
{
	None UMETA(DisplayName = "None"),
	RequestingPull UMETA(DisplayName = "RequestingPull"),
	Pulling UMETA(DisplayName = "Pulling"),
	Attached UMETA(DisplayName = "Attached"),
	Throwing UMETA(DisplayName = "Throwing"),
};

UCLASS()
class TANTRUMN_API ATantrumnCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATantrumnCharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void RequestSprintStart();
	virtual void RequestSprintEnd();

	void RequestThrowObject();
	void RequestPullObject();
	void RequestStopPullObject();
	void ResetThrowableObject();

	void OnThrowableAttached(AThrowableActor* InThrowableActor);

	bool CanThrowObject() const { return CharacterThrowState == ECharacterThrowState::Attached; }

	UFUNCTION(BlueprintPure)
	bool IsPullingObject() const { return CharacterThrowState == ECharacterThrowState::RequestingPull || CharacterThrowState == ECharacterThrowState::Pulling; }

	UFUNCTION(BlueprintPure)
	ECharacterThrowState GetCharacterThrowState() const { return CharacterThrowState; }

	UFUNCTION(BlueprintPure)
	bool IsStunned() const { return bIsStunned; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SphereCastPlayerView();
	void SphereCastActorTransform();
	void LineCastActorTransform();
	void ProcessTraceResult(const FHitResult& HitResult);

	bool PlayThrowMontage();
	void UnbindMontage();

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	
	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	void OnStunBegin(float StunRatio);
	void UpdateStun();
	void OnStunEnd();

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

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	USoundCue* HeavyLandSound = nullptr;

	float StunTime = 0.0f;
	float StunBeginTimeStamp = 0.0f;

	bool bIsStunned = false;
	bool bIsSprinting = false;

	float MaxWalkSpeed = 0.0f;






	UPROPERTY(VisibleAnywhere, Category = "Throw")
	ECharacterThrowState CharacterThrowState = ECharacterThrowState::None;

	UPROPERTY(VisibleAnywhere, Category = "Throw", meta = (ClampMin = "0", Unit = "ms"))
	float ThrowSpeed = 2000.0f; 

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* ThrowMontage = nullptr;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

private:

	UPROPERTY()
	AThrowableActor* ThrowableActor;

};
