// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	CD_Forward	UMETA(DisplayName = "Forward"),
	CD_Backward	UMETA(DisplayName = "Backward"),
	CD_Left		UMETA(DisplayName = "Left"),
	CD_Right	UMETA(DisplayName = "Right"),
};