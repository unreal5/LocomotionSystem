// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "LSBaseAnimInstance.generated.h"

struct FAnimNodeReference;
/**
 *	动画实例代理类 
 */
/**
 * 
 */
struct FLSBaseAnimInstanceProxy : public FAnimInstanceProxy
{
	FLSBaseAnimInstanceProxy(UAnimInstance* Instance);


protected:
	virtual void Update(float DeltaSeconds) override;
	virtual void PostUpdate(UAnimInstance* InAnimInstance) const override;
};


// 动画实例基类
UCLASS(Abstract)
class LOCOMOTIONSYSTEM_API ULSBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	// 基类默认实现调用delete函数，不需要重写
	// virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;

	// 一些公共函数及变量
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|Idle")
	TObjectPtr<UAnimSequenceBase> IdleAnim;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnUpdateIdleAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
private:
	FLSBaseAnimInstanceProxy& GetLSBaseAnimInstanceProxy();
	friend FLSBaseAnimInstanceProxy;
	
};
