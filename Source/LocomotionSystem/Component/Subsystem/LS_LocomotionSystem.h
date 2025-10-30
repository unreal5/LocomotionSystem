// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LS_LocomotionSystem.generated.h"


UCLASS(ClassGroup=("用户定制组件"), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract)
class LOCOMOTIONSYSTEM_API ULS_LocomotionSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	ULS_LocomotionSystem();
};
