// 版权没有，拷贝自由。


#include "LSBaseAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "SequencePlayerLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	if (OwnerCharacter.Get())
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
}

FAnimInstanceProxy* ULSBaseAnimInstance::CreateAnimInstanceProxy()
{
	return new FLSBaseAnimInstanceProxy(this);
}

void ULSBaseAnimInstance::UpdateLocationData()
{
	if (OwnerCharacter.IsValid() == false) return;

	WorldLocation = OwnerCharacter->GetActorLocation();
}

void ULSBaseAnimInstance::UpdateVelocityData()
{
	if (OwnerMovementComp.IsValid() == false) return;

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
	if (OwnerCharacter.IsValid() == false) return;
	WorldRotation = OwnerCharacter->GetActorRotation(); //WorldTransform.GetRotation().Rotator();
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
