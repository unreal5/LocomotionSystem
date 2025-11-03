// 版权没有，拷贝自由。


#include "LSBaseAnimInstance.h"

#include "AnimExecutionContextLibrary.h"
#include "KismetAnimationLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimExecutionContext.h"

FLSBaseAnimInstanceProxy::FLSBaseAnimInstanceProxy(UAnimInstance* Instance) : FAnimInstanceProxy(Instance)
{
}


FLSBaseAnimInstanceProxy& ULSBaseAnimInstance::GetLSBaseAnimInstanceProxy()
{
	return GetProxyOnGameThread<FLSBaseAnimInstanceProxy>();
}

void FLSBaseAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}

void FLSBaseAnimInstanceProxy::PostUpdate(UAnimInstance* InAnimInstance) const
{
	FAnimInstanceProxy::PostUpdate(InAnimInstance);
}

void ULSBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void ULSBaseAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	UpdateLocationData();
	UpdateRotationData();
	UpdateVelocityData();
	UpdateAccelerationData();
}

FAnimInstanceProxy* ULSBaseAnimInstance::CreateAnimInstanceProxy()
{
	return new FLSBaseAnimInstanceProxy(this);
}

UCharacterMovementComponent* ULSBaseAnimInstance::GetOwnerMovementComponent()
{
	if (!OwnerMovementComp)
	{
		if (!OwnerCharacter)
		{
			OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		}
		if (OwnerCharacter)
		{
			OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
		}
	}
	return OwnerMovementComp;
}

void ULSBaseAnimInstance::UpdateLocationData()
{
	if (!OwnerCharacter) return;

	WorldLocation = OwnerCharacter->GetActorLocation();
}

void ULSBaseAnimInstance::UpdateVelocityData()
{
	if (!OwnerMovementComp) return;

	bWasMovingLastUpdate = !LocalVelocity2D.IsNearlyZero();
	FVector Velocity = OwnerMovementComp->Velocity;
	WorldVelocity = Velocity;
	WorldVelocity2D = FVector(Velocity.X, Velocity.Y, 0.f);
	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);

	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity, WorldRotation);
	LocalVelocityDirectionNoOffset = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngle,
	                                                                  CardinalDirectDeadZone,
	                                                                  LocalVelocityDirectionNoOffset,
	                                                                  bWasMovingLastUpdate);
}

void ULSBaseAnimInstance::UpdateRotationData()
{
	if (!OwnerCharacter) return;
	// WorldRotation 持有上一帧的旋转数据。
	const FRotator CurrentFrameActorRotation = OwnerCharacter->GetActorRotation();
	YawDeltaSinceLastFrame = CurrentFrameActorRotation.Yaw - WorldRotation.Yaw;
	WorldRotation = CurrentFrameActorRotation;
	SetRootYawOffset(RootYawOffset-YawDeltaSinceLastFrame);
}

void ULSBaseAnimInstance::UpdateAccelerationData()
{
	if (!OwnerMovementComp) return;
	FVector WorldAcceleration2D = OwnerMovementComp->GetCurrentAcceleration() * FVector{1.f, 1.f, 0.f};
	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);
	bHasAcceleration = !LocalAcceleration2D.IsNearlyZero();
}

void ULSBaseAnimInstance::UpdateRootYawOffset(float DeltaSeconds)
{
	switch (RootYawOffsetMode)
	{
	case ERootYawOffsetMode::Accumulate:
		
		break;
	case ERootYawOffsetMode::BlendOut:
		break;
	case ERootYawOffsetMode::Hold:
		break;

	default:
		check(0);
		break;
	}
}

void ULSBaseAnimInstance::SetRootYawOffset(float InRootYawOffset)
{
}

ECardinalDirection ULSBaseAnimInstance::SelectCardinalDirectionFromAngle(float Angle, float DeadZone,
                                                                         ECardinalDirection CurrentDirection,
                                                                         bool bUseCurrentDirection)
{
	float FwdDeadZone = DeadZone;
	float BwdDeadZone = DeadZone;
	// Adjust dead zone based on current direction
	if (bUseCurrentDirection)
	{
		switch (CurrentDirection)
		{
		case ECardinalDirection::CD_Forward:
			FwdDeadZone = DeadZone * 2.f;
			break;
		case ECardinalDirection::CD_Backward:
			BwdDeadZone = DeadZone * 2.f;
			break;
		default:
			break;
		}
	}

	const float AbsAngle = FMath::Abs(Angle);
	if (AbsAngle <= FwdDeadZone + 45.f) return ECardinalDirection::CD_Forward;
	if (AbsAngle >= 135.f - BwdDeadZone) return ECardinalDirection::CD_Backward;

	return (Angle > 0.f) ? ECardinalDirection::CD_Right : ECardinalDirection::CD_Left;
}


void ULSBaseAnimInstance::OnUpdateIdleAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequencePlayerReference SeqPlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;


	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, IdleAnim);
}

void ULSBaseAnimInstance::OnSetupIdleBreakAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (IdleBreaks.Num() == 0) return;

	EAnimNodeReferenceConversionResult ConversionResult;
	FSequencePlayerReference SeqPlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;

	int32 Index = FMath::RandRange(0, IdleBreaks.Num() - 1);
	UAnimSequenceBase* IdleBreakAnim = IdleBreaks[Index];
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, IdleBreakAnim);
}

void ULSBaseAnimInstance::OnUpdateCycleAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequencePlayerReference SeqPlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;

	UAnimSequenceBase* CycleAnim = nullptr;
	switch (LocalVelocityDirectionNoOffset)
	{
	case ECardinalDirection::CD_Forward:
		CycleAnim = CycleCardinals.Forward;
		break;
	case ECardinalDirection::CD_Backward:
		CycleAnim = CycleCardinals.Backward;
		break;
	case ECardinalDirection::CD_Right:
		CycleAnim = CycleCardinals.Right;
		break;
	case ECardinalDirection::CD_Left:
		CycleAnim = CycleCardinals.Left;
		break;
	default:
		break;
	}
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, CycleAnim);
}

void ULSBaseAnimInstance::OnSetupTurnInPlaceAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequenceEvaluatorReference SeqEvalRef = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;

	TurnInPlaceAnimTime = 0.f;
	USequenceEvaluatorLibrary::SetExplicitTime(SeqEvalRef, TurnInPlaceAnimTime);
	bTurnInPlaceB = !bTurnInPlaceB;
}

void ULSBaseAnimInstance::OnUpdateTurnInPlaceAnimLayer(const FAnimUpdateContext& Context,
                                                       const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequenceEvaluatorReference SeqEvalRef = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;


	auto AnimSeq = SelectTurnInPlaceAnimation(TurnInPlaceRotationDirection);
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context, SeqEvalRef, AnimSeq);

	const float DeltaTime = UAnimExecutionContextLibrary::GetDeltaTime(Context);
	TurnInPlaceAnimTime += DeltaTime * TurnInPlaceAnims.PlayRate;
	USequenceEvaluatorLibrary::SetExplicitTime(SeqEvalRef, TurnInPlaceAnimTime);
}

void ULSBaseAnimInstance::OnSetupTurnInPlaceStateLayer(const FAnimUpdateContext& Context,
                                                       const FAnimNodeReference& Node)
{
	TurnInPlaceRotationDirection = -FMath::Sign(RootYawOffset);
}

void ULSBaseAnimInstance::OnUpdateTurnInPlaceRecoveryStateLayer(const FAnimUpdateContext& Context,
                                                                const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	FSequencePlayerReference SeqPlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;

	UAnimSequenceBase* SelectedRecoveryAnim = SelectTurnInPlaceAnimation(TurnInPlaceRotationDirection);
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, SelectedRecoveryAnim);
}

UAnimSequenceBase* ULSBaseAnimInstance::SelectTurnInPlaceAnimation(float Direction)
{
	return Direction > 0.f ? TurnInPlaceAnims.Right90 : TurnInPlaceAnims.Left90;
}
