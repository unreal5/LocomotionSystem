// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	BlendOut,
	Hold,
	Accumulate,
};