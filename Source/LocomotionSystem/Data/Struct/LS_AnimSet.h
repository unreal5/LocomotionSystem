// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "AnimCardinal.h"
#include "LS_AnimSet.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FLS_AnimSet : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画|Idle")
	TObjectPtr<UAnimSequenceBase> IdleAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|Idle")
	TArray<TObjectPtr<UAnimSequenceBase>> IdleBreaks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画|Walk")
	FAnimCardinal CycleCardinals;
};
