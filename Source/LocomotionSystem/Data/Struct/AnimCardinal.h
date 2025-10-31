// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "AnimCardinal.generated.h"
/**
 * 
 */
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
