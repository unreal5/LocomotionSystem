// 版权没有，拷贝自由。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Data/Enum/Locomotion/CardinalDirection.h"
#include "Data/Enum/Locomotion/RootYawOffsetMode.h"
#include "Data/Struct/LS_AnimSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "LSBaseAnimInstance.generated.h"

struct FAnimUpdateContext;
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

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UCharacterMovementComponent* GetOwnerMovementComponent();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|位置")
	FVector WorldLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|旋转")
	FRotator WorldRotation;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|旋转")
	float RootYawOffset;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|旋转")
	float YawDeltaSinceLastFrame;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|旋转")
	ERootYawOffsetMode RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	FVector WorldVelocity;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	FVector WorldVelocity2D;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	FVector LocalVelocity2D;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|速度")
	bool bWasMovingLastUpdate;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|方向")
	float LocalVelocityDirectionAngle;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|方向")
	ECardinalDirection LocalVelocityDirectionNoOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "动画|方向")
	float CardinalDirectDeadZone = 11.25f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|加速度")
	FVector LocalAcceleration2D;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "动画|加速度")
	bool bHasAcceleration;
	
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

	/* 原地转身 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|原地转身")
	FAnimSet_TurnInPlace TurnInPlaceAnims;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|原地转身")
	float TurnInPlaceAnimTime = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|原地转身")
	float TurnInPlaceRotationDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "动画|原地转身")
	bool bTurnInPlaceB = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnSetupTurnInPlaceAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnUpdateTurnInPlaceAnimLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnSetupTurnInPlaceStateLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	void OnUpdateTurnInPlaceRecoveryStateLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UPROPERTY(BlueprintReadWrite, Category = "动画|原地转身")
	FAnimSet_Config L_Config;
	
	/* 原地转身-Recovery */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线")
	FName Curve_TurnYawWeight = TEXT("TurnYawWeight");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线")
	FName Curve_RemainingTurnYaw = TEXT("RemainingTurnYaw");
private:
	FLSBaseAnimInstanceProxy& GetLSBaseAnimInstanceProxy();

	ACharacter* OwnerCharacter;
	UCharacterMovementComponent* OwnerMovementComp;

	void UpdateLocationData();
	void UpdateVelocityData();
	void UpdateRotationData();
	void UpdateAccelerationData();
	void UpdateRootYawOffset(float DeltaSeconds);
	// 内部辅助函数
	void SetRootYawOffset(float InRootYawOffset);
	
	ECardinalDirection SelectCardinalDirectionFromAngle(float Angle, float DeadZone,
	                                                    ECardinalDirection CurrentDirection,
	                                                    bool bUseCurrentDirection = true);


	UAnimSequenceBase* SelectTurnInPlaceAnimation(float Direction);

	FFloatSpringState RootYawOffsetSpringState;
	
private:
	friend FLSBaseAnimInstanceProxy;
};
