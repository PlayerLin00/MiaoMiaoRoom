// Fill out your copyright notice in the Description page of Project Settings.


#include "PyInterSubSystem.h"
#include "Engine/Engine.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
void UPyInterSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("PyInterSubSystem Initialized"));
}

void UPyInterSubSystem::Deinitialize()
{
	if (WebSocket.IsValid())
	{
		DisconnectWebSocket();
	}
	Super::Deinitialize();
}

int UPyInterSubSystem::testFunc(int a, int b) 
{
	return a + b;
}

void UPyInterSubSystem::ConnectWebSocket(const FString& ServerURL)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		UE_LOG(LogTemp, Warning, TEXT("WebSocket is already connected"));
		return;
	}

	// Create WebSocket using correct UE5.5 API from documentation
	FString ServerURLTest = TEXT("ws://localhost:8000/ws/unreal_client");
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURLTest);

	if (!WebSocket.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create WebSocket"));
		return;
	}

	// Bind WebSocket events using lambda expressions for better compatibility
	WebSocket->OnConnected().AddLambda([this]()
	{
		UE_LOG(LogTemp, Warning, TEXT("WebSocket connected successfully"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("WebSocket Connected!"));
		}
	});
	
	WebSocket->OnConnectionError().AddLambda([this](const FString& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("WebSocket connection error: %s"), *Error);
		UE_LOG(LogTemp, Error, TEXT("Detailed error information - Check if server supports WebSocket protocol"));
		UE_LOG(LogTemp, Error, TEXT("Common causes: 1) Server not running 2) Wrong path 3) Server not WebSocket-enabled 4) Firewall blocking"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("WebSocket Error: %s"), *Error));
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Check server status and WebSocket support"));
		}
	});
	
	WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)
	{
		UE_LOG(LogTemp, Warning, TEXT("WebSocket closed. Status: %d, Reason: %s, Clean: %s"), 
			StatusCode, *Reason, bWasClean ? TEXT("true") : TEXT("false"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("WebSocket Disconnected"));
		}
	});
	
	WebSocket->OnMessage().AddLambda([this](const FString& Message)
	{
		UE_LOG(LogTemp, Warning, TEXT("Received WebSocket message: %s"), *Message);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Received: %s"), *Message));
		}
	});

	WebSocket->Connect();
	UE_LOG(LogTemp, Warning, TEXT("Attempting WebSocket connection to: %s"), *ServerURL);
	UE_LOG(LogTemp, Warning, TEXT("Make sure the server at localhost:8000 supports WebSocket protocol and path /ws/ue_client_01 exists"));
}

void UPyInterSubSystem::DisconnectWebSocket()
{
	if (WebSocket.IsValid())
	{
		if (WebSocket->IsConnected())
		{
			WebSocket->Close();
		}
		WebSocket.Reset();
		UE_LOG(LogTemp, Warning, TEXT("WebSocket disconnected"));
	}
}

void UPyInterSubSystem::SendMessage(const FString& Message)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(Message);
		UE_LOG(LogTemp, Warning, TEXT("Sent message: %s"), *Message);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WebSocket is not connected. Cannot send message."));
	}
}

bool UPyInterSubSystem::IsConnected() const
{
	return WebSocket.IsValid() && WebSocket->IsConnected();
}

// JSON��Ϣ�����ͷ��͹���ʵ��
void UPyInterSubSystem::SendCreateNPCMessage(const FString& NPCId, const FString& InitContext)
{
	FCreateNPCMessage Message;
	Message.npc_id = NPCId;
	Message.init_context = InitContext;
	Message.timestamp = GetCurrentTimestamp();
	
	FString JsonString = CreateNPCMessageToJson(Message);
	SendMessage(JsonString);
	
	UE_LOG(LogTemp, Warning, TEXT("Sent CreateNPC JSON: %s"), *JsonString);
}

FString UPyInterSubSystem::CreateNPCMessageToJson(const FCreateNPCMessage& Message)
{
	FString OutputString;
	if (FJsonObjectConverter::UStructToJsonObjectString(Message, OutputString))
	{
		return OutputString;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert CreateNPCMessage to JSON"));
		return TEXT("");
	}
}

FString UPyInterSubSystem::StructToJson(const FString& StructName, const FString& StructData)
{
	// �����������ͨ�õĽṹ�嵽JSONת��
	// ����UE5�ķ���ϵͳ���ƣ������ṩһ���򻯵�ʵ��
	UE_LOG(LogTemp, Warning, TEXT("StructToJson called with: %s"), *StructName);
	return StructData;
}

double UPyInterSubSystem::GetCurrentTimestamp() const
{
	// ����Unixʱ������룩
	return FDateTime::UtcNow().ToUnixTimestamp();
}