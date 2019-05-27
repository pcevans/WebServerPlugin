// Fill out your copyright notice in the Description page of Project Settings.


#include "WebServer.h"


void AWebServer::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ShutdownServer();
}


void AWebServer::StartServer()
{
	//RegisterHTMLResponses();

	FIPv4Endpoint Endpoint(FIPv4Address(127,0,0,1), 8080);
	FString SocketName = "localhost";
	ListenerSocket = FTcpSocketBuilder(*SocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	int32 NewSize = 0;
	ListenerSocket->SetReceiveBufferSize(2*1024*1024, NewSize);

	GetWorldTimerManager().SetTimer(ListenerTimer,this,
		&AWebServer::ListenerSocketLoop, 1.0, true);

	GetWorldTimerManager().SetTimer(ConnectionTimer, this,
		&AWebServer::ConnectionSocketLoop, 0.05, true);
}

void AWebServer::ShutdownServer()
{
	ListenerSocket->Close();
	ListenerSocket = nullptr;
	for (FSocket* ConnectionSocket : ConnectionSockets) {
		ConnectionSocket->Close();
		ConnectionSockets.Remove(ConnectionSocket);
	}
}

void AWebServer::RegisterURL(FString URL, FString Verb, const FOnRequestReceived& URLCallback, bool bCaptureSubDirectoryURLs)
{
	URLCallbacks.Add(URL, URLCallback);
	URLOverrides.Add(URL, bCaptureSubDirectoryURLs);
}

void AWebServer::ListenerSocketLoop()
{
	//~~~~~~~~~~~~~
	if (!ListenerSocket) return;
	//~~~~~~~~~~~~~

	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// handle incoming connections
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		//New Connection receive!
		FSocket* ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("Received new socket connection!"));
		//ConnectionSocket->

		if (ConnectionSocket != NULL)
		{
			//Global cache of current connection & remote address
			ConnectionSockets.Add(ConnectionSocket);
			//ConnectionAddresses.Add(FIPv4Endpoint(RemoteAddress));
		}
	}
}

void AWebServer::ConnectionSocketLoop()
{
	//~~~~~~~~~~~~~
	static int32 frame = 0;
	if (ConnectionSockets.Num() <= frame)
		frame = 0;
	if (ConnectionSockets.Num() == 0)
	{
		return;
	}
	FSocket* ConnectionSocket = ConnectionSockets[frame];
	if (ConnectionSocket->GetConnectionState() != SCS_Connected)
	{
		ConnectionSockets.Remove(ConnectionSocket);
		return;
	}
	frame++;
	//~~~~~~~~~~~~~

	//Binary data buffer for connection
	TArray<uint8> ReceivedData;

	uint32 Size;
	if (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(0, FMath::Min(Size, 65507u));

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

		UConnection *Connection = NewObject<UConnection>(this);
		Connection->Initialize(ConnectionSocket,ReceivedData);
		ConnectionSockets.Remove(ConnectionSocket);
		FString RegisteredURL = GetRegisteredURL(Connection->GetRequestSubDirectories());
		if (RegisteredURL.Len() > 0)
		{
			URLCallbacks.Find(RegisteredURL)->ExecuteIfBound(Connection);
		}
		else
		{
			//Connection->SendSimpleHTMLResponse(200,"<!DOCTYPE html><html><body><h1>My First Web Server</h1><p>You have been served.</p></body></html>");
			Connection->SendSimpleHTMLResponse(404, "<!DOCTYPE html><html><body><h1>404</h1><p>No page exists at " + Connection->GetRequestFileURL() + " or " + Connection->GetRequestBaseDirectory() + ".</p></body></html>");
		}
		UE_LOG(LogTemp,Warning,TEXT("Data Read! %d"), ReceivedData.Num());
	}
	else
	{
		ConnectionSocket->Close();
		ConnectionSockets.Remove(ConnectionSocket);
		UE_LOG(LogTemp, Warning, TEXT("Removed another empty requester..."));
	}


	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Bytes read %d from connection %d"), ReceivedData.Num(), frame));
}

FString AWebServer::GetRegisteredURL(TArray<FString> SubDirectories)
{

	FString SubDirectoryBuilder = "";
	for (int i = 0; i < SubDirectories.Num(); i++)
	{
		SubDirectoryBuilder += SubDirectories[i];
		if (URLCallbacks.Contains(SubDirectoryBuilder) && (*URLOverrides.Find(SubDirectoryBuilder) == true ||
			i == SubDirectories.Num() - 1))
		{
			return SubDirectoryBuilder;
		}
	}
	return "";
}


