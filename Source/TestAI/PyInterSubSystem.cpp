// Fill out your copyright notice in the Description page of Project Settings.


#include "PyInterSubSystem.h"
#include "Engine/Engine.h"
//#include "WebSocketsModule.h"
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
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, TEXT("ws"));

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
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("WebSocket Error: %s"), *Error));
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
	UE_LOG(LogTemp, Warning, TEXT("Connecting to WebSocket: %s"), *ServerURL);
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

