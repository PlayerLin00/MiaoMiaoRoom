// Fill out your copyright notice in the Description page of Project Settings.

#include "AINPC.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"

AAINPC::AAINPC()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 初始化重试参数
	RetryCount = 0;
	MaxRetryCount = 10;
	RetryInterval = 1.0f;
	bMessageSent = false;
	
	// 设置默认NPCId
	NPCId = FString::Printf(TEXT("NPC_%d"), GetUniqueID());
	InitContext = TEXT("Default NPC initialization context");
}

void AAINPC::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("NPC %s BeginPlay started"), *NPCId);
	
	// 尝试发送创建NPC消息
	AttemptSendMessage();
}

void AAINPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 清理定时器
	if (GetWorld() && RetryTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandle);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AAINPC::SendCreateNPCMessage()
{
	if (UPyInterSubSystem* PyInterSubSystem = GetGameInstance()->GetSubsystem<UPyInterSubSystem>())
	{
		if (PyInterSubSystem->IsConnected())
		{
			PyInterSubSystem->SendCreateNPCMessage(NPCId, InitContext);
			bMessageSent = true;
			StopRetrying();
			
			UE_LOG(LogTemp, Warning, TEXT("NPC %s successfully sent create message"), *NPCId);
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, 
					FString::Printf(TEXT("NPC %s connected to server"), *NPCId));
			}
		}
	}
}

bool AAINPC::TrySendMessage()
{
	if (bMessageSent)
	{
		return true;
	}
	
	if (UPyInterSubSystem* PyInterSubSystem = GetGameInstance()->GetSubsystem<UPyInterSubSystem>())
	{
		if (PyInterSubSystem->IsConnected())
		{
			SendCreateNPCMessage();
			return true;
		}
	}
	
	return false;
}

void AAINPC::AttemptSendMessage()
{
	if (bMessageSent)
	{
		return;
	}
	
	if (TrySendMessage())
	{
		return;
	}
	
	// 如果发送失败且还有重试次数
	if (RetryCount < MaxRetryCount)
	{
		RetryCount++;
		UE_LOG(LogTemp, Warning, TEXT("NPC %s: WebSocket not connected, retrying in %f seconds (attempt %d/%d)"), 
			*NPCId, RetryInterval, RetryCount, MaxRetryCount);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
				FString::Printf(TEXT("NPC %s waiting for connection... (%d/%d)"), *NPCId, RetryCount, MaxRetryCount));
		}
		
		// 设置重试定时器
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(RetryTimerHandle, 
				FTimerDelegate::CreateUObject(this, &AAINPC::AttemptSendMessage), 
				RetryInterval, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NPC %s: Failed to send create message after %d attempts"), *NPCId, MaxRetryCount);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
				FString::Printf(TEXT("NPC %s failed to connect after %d attempts"), *NPCId, MaxRetryCount));
		}
	}
}

void AAINPC::StopRetrying()
{
	if (GetWorld() && RetryTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandle);
		RetryTimerHandle.Invalidate();
	}
}