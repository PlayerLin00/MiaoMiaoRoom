// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IWebSocket.h"
#include "PyInterSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class TESTAI_API UPyInterSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "MyUtils")
	int testFunc(int a, int b);

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void ConnectWebSocket(const FString& ServerURL);

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void DisconnectWebSocket();

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SendMessage(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	bool IsConnected() const;

private:
	TSharedPtr<IWebSocket> WebSocket;
};
