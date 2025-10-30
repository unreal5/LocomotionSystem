// 版权没有，拷贝自由。


#include "LSBaseAnimInstance.h"

#include "SequencePlayerLibrary.h"

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

FAnimInstanceProxy* ULSBaseAnimInstance::CreateAnimInstanceProxy()
{
	return new FLSBaseAnimInstanceProxy(this);
}

void ULSBaseAnimInstance::OnUpdateIdleAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult  ConversionResult;
	FSequencePlayerReference SeqPlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;


	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, IdleAnim);
}

void ULSBaseAnimInstance::OnSetupIdleBreakAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (IdleBreaks.Num() == 0) return;
	
	EAnimNodeReferenceConversionResult  ConversionResult;
	FSequencePlayerReference SeqPlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConversionResult);
	if (ConversionResult != EAnimNodeReferenceConversionResult::Succeeded) return;

	int32 Index = FMath::RandRange(0, IdleBreaks.Num() - 1);
	UAnimSequenceBase* IdleBreakAnim = IdleBreaks[Index];
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SeqPlayerRef, IdleBreakAnim);
}
