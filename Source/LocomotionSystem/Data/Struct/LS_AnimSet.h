// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "LS_AnimSet.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FLS_AnimSet : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画集")
	TObjectPtr<UAnimSequenceBase> IdleAnim;
};
