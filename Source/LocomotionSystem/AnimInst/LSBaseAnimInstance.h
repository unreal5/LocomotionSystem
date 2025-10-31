// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Data/Struct/AnimCardinal.h"
#include "LSBaseAnimInstance.generated.h"

class UCharacterMovementComponent;
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

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|位置")
	FVector WorldLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|旋转")
	FRotator WorldRotation;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	FVector WorldVelocity;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	FVector2D WorldVelocity2D;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	float LocalVelocityDirectionAngle;
	
	// 一些公共函数及变量
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|Idle")
	TObjectPtr<UAnimSequenceBase> IdleAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|Idle")
	TArray<TObjectPtr<UAnimSequenceBase>> IdleBreaks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|Idle")
	float TimeUntilNextIdleBreak = 3.f;
	

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnUpdateIdleAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnSetupIdleBreakAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	// Cycle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|Cycle")
	FAnimCardinal CycleCardinals;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnUpdateCycleAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

private:
	FLSBaseAnimInstanceProxy& GetLSBaseAnimInstanceProxy();

public:


private:
	TWeakObjectPtr<ACharacter> OwnerCharacter;
	TWeakObjectPtr<UCharacterMovementComponent> OwnerMovementComp;

	void UpdateLocationData();
	void UpdateVelocityData();
	void UpdateRotationData();

private:
	friend FLSBaseAnimInstanceProxy;
};
