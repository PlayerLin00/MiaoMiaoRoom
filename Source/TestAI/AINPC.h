// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PyInterSubSystem.h"
#include "Engine/TimerHandle.h"
#include "AINPC.generated.h"

UCLASS()
class TESTAI_API AAINPC : public ACharacter
{
	GENERATED_BODY()

public:
	AAINPC();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// NPC基本属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Settings")
	FString NPCId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Settings")
	FString InitContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Settings")
	FString ConfigFileName;

	// WebSocket消息发送
	UFUNCTION(BlueprintCallable, Category = "NPC Communication")
	void SendCreateNPCMessage();

	UFUNCTION(BlueprintCallable, Category = "NPC Communication")
	void SendDeleteNPCMessage();

	UFUNCTION(BlueprintCallable, Category = "NPC Communication")
	bool TrySendMessage();

private:
	// 重试机制
	FTimerHandle RetryTimerHandle;
	int32 RetryCount;
	int32 MaxRetryCount;
	float RetryInterval;

	// 消息发送状态
	bool bMessageSent;

	void AttemptSendMessage();
	void StopRetrying();
};