// 版权没有，拷贝自由。


#include "LS_LocomotionSystem.h"


ULS_LocomotionSystem::ULS_LocomotionSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FLS_AnimSet ULS_LocomotionSystem::GetAnimSet(FName RowName) const
{
	if (!IsValid(AnimSetDataTable)) return FLS_AnimSet();

	auto AnimSet = AnimSetDataTable->FindRow<FLS_AnimSet>(RowName, TEXT("ULS_LocomotionSystem::GetAnimSet"));
	return AnimSet ? *AnimSet : FLS_AnimSet();
}
