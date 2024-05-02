// Fill out your copyright notice in the Description page of Project Settings.


#include "TantrumnCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TantrumnPlayerController.h"
#include "ThrowableActor.h"

#include "DrawDebugHelpers.h"
//using namespace AThrowableActor;

constexpr int CVSphereCastPlayerView = 0;
constexpr int CVSphereCastActorTransform = 1;
constexpr int CVSphereLineCastActorTransform = 2;


/*static TAutoConsoleVariable<int> CVarTraceMode(
	TEXT("Tantrumn.Character.Debug.TraceMode"),
	0,
	TEXT("	  0: Sphere cast PlayerView is used for direction/rotation (default).\n")
	TEXT("    1: Sphere cast using ActorTransform \n")
	TEXT("    2: Line cast using ActorTransform \n"),
	ECVF_Default);


static TAutoConsoleVariable<bool> CVarTraceMode(
	TEXT("Tantrumn.Character.Debug.DisplayTrace"),
	false,
	TEXT("Display Trace"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarTraceMode(
	TEXT("Tantrumn.Character.Debug.DisplayThrowVelocity"),
	false,
	TEXT("Display ThrowVelocity"),
	ECVF_Default);*/

// Sets default values
ATantrumnCharacterBase::ATantrumnCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATantrumnCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
}

// Called every frame
void ATantrumnCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateStun();
	if (bIsStunned)
	{
		return;
	}

	if (CharacterThrowState == ECharacterThrowState::Throwing)

	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (UAnimMontage* CurrentAnimMontage = AnimInstance->GetCurrentActiveMontage())
			{
				const float PlayRate = AnimInstance->GetCurveValue(TEXT("ThrowCurve"));
				AnimInstance->Montage_SetPlayRate(CurrentAnimMontage, PlayRate);
			}	
		}
	}
	else if (CharacterThrowState == ECharacterThrowState::None || CharacterThrowState == ECharacterThrowState::RequestingPull)
	{
		/*switch (CVarTraceMode->GetInt())
		{
		case CVSphereCastPlayerView:
			SphereCastPlayerView();
			break;
		}*/
	}
}

// Called to bind functionality to input
void ATantrumnCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATantrumnCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//custom landed code
	ATantrumnPlayerController* TantrumnPlayerController = GetController<ATantrumnPlayerController>();
	if (TantrumnPlayerController)
	{
		const float FallImpactSpeed = FMath::Abs(GetVelocity().Z);
		if (FallImpactSpeed < MinImpactSpeed)
		{
			//nothing to do, very light fall
			return;
		}
		else
		{
			if (HeavyLandSound && GetOwner())
			{
				FVector CharacterLocation = GetOwner()->GetActorLocation();
				UGameplayStatics::PlaySoundAtLocation(this, HeavyLandSound, CharacterLocation);
			}
		}


		const float DeltaImpact = MaxImpactSpeed - MinImpactSpeed;
		const float FallRatio = FMath::Clamp( (FallImpactSpeed - MinImpactSpeed) / DeltaImpact, 0.0f, 1.0f);
		const bool bAffectSmall = FallRatio <= 0.5f;
		const bool bAffectLarge = FallRatio > 0.5f;
		TantrumnPlayerController->PlayDynamicForceFeedback(FallRatio, 0.5f, bAffectLarge, bAffectSmall, bAffectLarge, bAffectSmall);

		if (bAffectLarge)
		{
			OnStunBegin(FallRatio);
		}
	}
}

void ATantrumnCharacterBase::RequestSprintStart()
{
	if (!bIsStunned)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}

}
void ATantrumnCharacterBase::RequestSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ATantrumnCharacterBase::RequestThrowObject()
{
	if (CanThrowObject())
	{
		if (PlayThrowMontage())
		{
			CharacterThrowState = ECharacterThrowState::Throwing;
		}
		else
		{
			ResetThrowableObject();
		}
	}
}

void ATantrumnCharacterBase::RequestPullObject()
{
	if (!bIsStunned && CharacterThrowState == ECharacterThrowState::None)
	{
		CharacterThrowState = ECharacterThrowState::RequestingPull;
	}
}

void ATantrumnCharacterBase::RequestStopPullObject()
{
	if (CharacterThrowState == ECharacterThrowState::RequestingPull)
	{
		CharacterThrowState = ECharacterThrowState::None;
	}
}

void ATantrumnCharacterBase::ResetThrowableObject()
{
	if (ThrowableActor)
	{
		ThrowableActor->Drop();
	}
	CharacterThrowState = ECharacterThrowState::None;
	ThrowableActor = nullptr;
}

void ATantrumnCharacterBase::OnThrowableAttached(AThrowableActor* InThrowableActor)
{
	CharacterThrowState = ECharacterThrowState::Attached;
	ThrowableActor = InThrowableActor;
	MoveIgnoreActorAdd(ThrowableActor);
}

void ATantrumnCharacterBase::SphereCastPlayerView()
{
	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);
	const FVector PlayerViewForward = Rotation.Vector();
	const float AdditionalDistance = (Location - GetActorLocation()).Size();
	FVector EndPos = Location + (PlayerViewForward * (1000.0f + AdditionalDistance));

	const FVector CharacterForward = GetActorForwardVector();
	const float DotResult = FVector::DotProduct(PlayerViewForward, CharacterForward);

	if (DotResult < -0.23f)
	{
		if (ThrowableActor)
		{
			ThrowableActor->ToggleHighlight(false);
			ThrowableActor = nullptr;
		}
		return;
		//UE_LOG(LogTemp, Warning, TEXT("Dot Result: %f"), DotResult);
	}

	
	FHitResult HitResult;
	//EDrawDebugTrace::Type DebugTrace = CVarDisplayTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	//UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Location, EndPos, 70.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), );
	ProcessTraceResult(HitResult);

#if ENABLE_DRAW_DEBUG
	//if (CVarDisplayTrace->GetBool())
	{
		static float FovDeg = 90.0f;
		DrawDebugCamera(GetWorld(), Location, Rotation, FovDeg);
		DrawDebugLine(GetWorld(), Location, EndPos, HitResult.bBlockingHit ? FColor::Red : FColor::White);
		DrawDebugPoint(GetWorld(), EndPos, 70.0f, HitResult.bBlockingHit ? FColor::Red : FColor::White);
	}
#endif

}

void ATantrumnCharacterBase::SphereCastActorTransform()
{
	FVector StartPos = GetActorLocation();
	FVector EndPos = StartPos + (GetActorForwardVector() * 1000.0f);

	//EDrawDebugTrace::Type DebugTrace = CVarDisplayTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	FHitResult HitResult;
	//UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartPos, EndPos, 70.0f, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), );
	ProcessTraceResult(HitResult);
}

void ATantrumnCharacterBase::LineCastActorTransform()
{

	FVector StartPos = GetActorLocation();
	FVector EndPos = StartPos + (GetActorForwardVector() * 1000.0f);
	FHitResult HitResult;
	GetWorld() ? GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility) : false;
	#if ENABLE_DRAW_DEBUG
		//if (CVarDisplayTrace->GetBool())
		{
			DrawDebugLine(GetWorld(), StartPos, EndPos, HitResult.bBlockingHit ? FColor::Red : FColor::White, false);
		}
	#endif
		ProcessTraceResult(HitResult);
}

void ATantrumnCharacterBase::ProcessTraceResult(const FHitResult& HitResult)
{
	AThrowableActor* HitThrowableActor = HitResult.bBlockingHit ? Cast<AThrowableActor>(HitResult.GetActor()) : nullptr;
	const bool IsSameActor = (ThrowableActor == HitThrowableActor);
	const bool IsValidTarget = HitThrowableActor && HitThrowableActor->IsIdle();

	if (ThrowableActor)
	{
		if (!IsValidTarget || !IsSameActor)
		{
			ThrowableActor->ToggleHighlight(false);
			ThrowableActor = nullptr;
		}
	}

	if (IsValidTarget)
	{
		if (!ThrowableActor)
		{
			ThrowableActor = HitThrowableActor;
			//ThrowableActor = ToggleHighlight(true);
		}
	}

	if (CharacterThrowState == ECharacterThrowState::RequestingPull)
	{
		if (GetVelocity().SizeSquared() < 100.0f)
		{
			if (ThrowableActor && ThrowableActor->Pull(this))
			{
				CharacterThrowState = ECharacterThrowState::Pulling;
				ThrowableActor = nullptr;
			}
		}
	}

}

bool ATantrumnCharacterBase::PlayThrowMontage()
{
	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = PlayAnimMontage(ThrowMontage, PlayRate) > 0.f;
	if (bPlayedSuccessfully)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (!BlendingOutDelegate.IsBound())
		{
			BlendingOutDelegate.BindUObject(this, &ATantrumnCharacterBase::OnMontageBlendingOut);
		}
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, ThrowMontage);

		//AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ATantrumnCharacterBase::OnNotifyBeginRecieved);
		//AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ATantrumnCharacterBase::OnNotifyBeginRecieved);

	}

	return bPlayedSuccessfully;
}

void ATantrumnCharacterBase::UnbindMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		//AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ATantrumnCharacterBase::OnNotifyBeginRecieved);
		//AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ATantrumnCharacterBase::OnNotifyBeginRecieved);
	}
}

void ATantrumnCharacterBase::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}

void ATantrumnCharacterBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindMontage();
	CharacterThrowState = ECharacterThrowState::None;
	MoveIgnoreActorRemove(ThrowableActor),
	/*if (ThrowableActor->GetRootComponent())
	{
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ThrowableActor->GetRootComponent());
		if (RootPrimitiveComponent)
		{
			RootPrimitiveComponent->IgnoreActorWhenMoving(this, false);
		}
	}*/
	ThrowableActor = nullptr;
}

void ATantrumnCharacterBase::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& FBranchingPointNotifyPayload)
{
	if (ThrowableActor->GetRootComponent())
	{
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ThrowableActor->GetRootComponent());
		if (RootPrimitiveComponent)
		{
			RootPrimitiveComponent->IgnoreActorWhenMoving(this, true);
		}
	}
	const FVector& Direction = GetActorForwardVector() * ThrowSpeed;
	ThrowableActor->Launch(Direction);

	//if (CVarDisplayThrowVelocity->GetBool())
	{
		const FVector& Start = GetMesh()->GetSocketLocation(TEXT("ObjectAttach"));
		DrawDebugLine(GetWorld(), Start, Start + Direction, FColor::Red, false, 5.0f);
	}
} 

void ATantrumnCharacterBase::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& FBranchingPointNotifyPayload)
{

}

void ATantrumnCharacterBase::OnStunBegin(float StunRatio)
{
	if (bIsStunned)
	{
		//for now just early exit, alternative option would be to add to the stun time
		return;
	}

	const float StunDelt = MaxStunTime - MinStunTime;
	StunTime = MinStunTime + (StunRatio * StunDelt);
	StunBeginTimeStamp = FApp::GetCurrentTime();
}

void ATantrumnCharacterBase::OnStunEnd()
{
	return;
}
