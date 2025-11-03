// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "LS_AnimSet.generated.h"
/* Cardinal direction animation set */

USTRUCT(BlueprintType)
struct FAnimCardinal
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimSequenceBase> Forward;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimSequenceBase> Backward;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimSequenceBase> Left;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimSequenceBase> Right;
};

/* Turning in place animation set */
USTRUCT(BlueprintType)
struct FAnimSet_TurnInPlace
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimSequenceBase> Left90;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimSequenceBase> Right90;
	UPROPERTY(EditDefaultsOnly)
	float PlayRate = 1.f;
};

/* General animation set */
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画|原地转身")
	FAnimSet_TurnInPlace TurnInPlaceAnims;
};
