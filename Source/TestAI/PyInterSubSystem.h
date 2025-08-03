// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "JsonObjectConverter.h"
#include "PyInterSubSystem.generated.h"



// ��ӦPython�˵�CreateNPCMessage
USTRUCT(BlueprintType)
struct FCreateNPCMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString type = TEXT("create_npc");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString npc_id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString init_context;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double timestamp = 0.0;
};

// ��ӦPython�˵�PerceiveSceneMessage
USTRUCT(BlueprintType)
struct FPerceiveSceneMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString type = TEXT("perceive_scene");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString request_id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FString> filter_types;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float max_distance = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double timestamp = 0.0;
};

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

	//JSON��Ϣ�����ͷ��͹���
	UFUNCTION(BlueprintCallable, Category = "JSON Messages")
	void SendCreateNPCMessage(const FString& NPCId, const FString& InitContext);

	UFUNCTION(BlueprintCallable, Category = "JSON Messages")
	FString CreateNPCMessageToJson(const FCreateNPCMessage& Message);

	// ͨ��JSON���л�����
	UFUNCTION(BlueprintCallable, Category = "JSON Utils")
	FString StructToJson(const FString& StructName, const FString& StructData);

	// ��ȡ��ǰʱ���
	UFUNCTION(BlueprintCallable, Category = "Utils")
	double GetCurrentTimestamp() const;
private:
	TSharedPtr<IWebSocket> WebSocket;
};
