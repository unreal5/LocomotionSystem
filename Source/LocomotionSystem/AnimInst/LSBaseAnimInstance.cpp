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

	FVector Velocity = OwnerMovementComp->Velocity;
	WorldVelocity = Velocity;
	WorldVelocity2D = FVector2D(Velocity.X, Velocity.Y);

	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity, WorldRotation);
}

void ULSBaseAnimInstance::UpdateRotationData()
{
	if (OwnerCharacter.IsValid() == false) return;
	WorldRotation = OwnerCharacter->GetActorRotation();//WorldTransform.GetRotation().Rotator();
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

	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, CycleCardinals.Forward);
}
