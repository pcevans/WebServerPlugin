// Fill out your copyright notice in the Description page of Project Settings.


#include "WebServer.h"


void AWebServer::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ShutdownServer();
}


void AWebServer::StartServer()
{
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

void AWebServer::RegisterCallbackAtURL(FString URL, const FOnRequestReceived& URLCallback, bool bCaptureSubDirectoryURLs)
{
	URLCallbacks.Add(URL, URLCallback);
	SubURLCaptureAllowed.Add(URL, bCaptureSubDirectoryURLs);
}

void AWebServer::RegisterHTMLAtURL(FString URL, FString HTMLString, bool bCaptureSubDirectoryURLs)
{
	URLHTMLResponses.Add(URL, HTMLString);
	SubURLCaptureAllowed.Add(URL, bCaptureSubDirectoryURLs);
}

void AWebServer::RegisterMIMETypeForExtension(FString Extension, FString MIMEType)
{
	MIMETypeOverrides.Add(Extension, MIMEType);
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
		FSocket* ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("Web Server Connection"));
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
	//~~~~~Update which connection we will check this frame~~~~~~~~
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
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	static FString RootFolderPath = FPaths::ProjectContentDir() + "WebServerRoot";

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

		FOnRequestReceived *RegisteredCallback = GetRegisteredCallback(Connection->GetRequestSubDirectories());
		if (RegisteredCallback != nullptr)
		{
			RegisteredCallback->ExecuteIfBound(Connection);
		}
		else
		{
			FString RegisteredHTMLResponse = GetRegisteredHTMLResponse(Connection->GetRequestSubDirectories());
			if (RegisteredHTMLResponse != "")
			{
				Connection->SendSimpleHTMLResponse(200, *RegisteredHTMLResponse);
			}
			else
			{
				bool bFileLoadSuccess = false;
				FString FilePath = RootFolderPath + Connection->GetRequestFileURL();
				if (FilePath.Len() > 0)
				{
					FString PathSansExtension, Extension;
					if (FilePath.Split(".", &PathSansExtension, &Extension, ESearchCase::IgnoreCase, ESearchDir::FromEnd) && Extension.Len() > 0 && Extension.Len() < 6)
					{
						TArray<uint8> EncodedFile;
						FString ContentType;

						TArray<uint8> FileByteArray = *UDataConversionLibrary::ConvertFileToByteArray(FilePath);
						if (FileByteArray.Num() != 0)
						{
							Connection->SetResponseCode(200);
							if (MIMETypeOverrides.Contains(Extension))
							{
								Connection->SetResponseHeader("Content-Type", MIMETypeOverrides[Extension]);
							}
							else
							{
								Connection->SetResponseHeader("Content-Type", UHTTPMessageLibrary::ConvertFileTypeToMIMEType(Extension));
							}
							Connection->SetResponseHeader("Content-Length", FString::FromInt(FileByteArray.Num()));
							Connection->SetResponseBody(FileByteArray);
							Connection->SendResponse();
							bFileLoadSuccess = true;
						}
					}
				}
				if (!bFileLoadSuccess)
				{
					Connection->SendSimpleHTMLResponse(404, "<!DOCTYPE html><html><body><h1>404</h1><p>No page exists at " + Connection->GetRequestFileURL() + " or " + Connection->GetRequestBaseDirectory() + ".</p></body></html>");
				}
			}
			/*else
			{
				Connection->SendSimpleHTMLResponse(404, "<!DOCTYPE html><html><body><h1>404</h1><p>No page exists at " + Connection->GetRequestFileURL() + " or " + Connection->GetRequestBaseDirectory() + ".</p></body></html>");
			}*/
		}

		UE_LOG(LogTemp,Warning,TEXT("Data read! %d"), ReceivedData.Num());
	}
	else
	{
		ConnectionSocket->Close();
		ConnectionSockets.Remove(ConnectionSocket);
		UE_LOG(LogTemp, Warning, TEXT("Removed another empty requester..."));
	}

	FString ReceiveDataDebugMessage = FString::Printf(TEXT("Bytes read %d from connection %d"), ReceivedData.Num(), frame);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *ReceiveDataDebugMessage));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ReceiveDataDebugMessage);
}

FOnRequestReceived *AWebServer::GetRegisteredCallback(TArray<FString> SubDirectories)
{
	FString SubDirectoryBuilder = "";
	for (int i = 0; i < SubDirectories.Num(); i++)
	{
		SubDirectoryBuilder += SubDirectories[i];
		if (URLCallbacks.Contains(SubDirectoryBuilder) &&
			(*SubURLCaptureAllowed.Find(SubDirectoryBuilder) == true || i == SubDirectories.Num() - 1))
		{
			return URLCallbacks.Find(SubDirectoryBuilder);
		}
		else if (i != 0 && i != SubDirectories.Num() - 1)
		{
			SubDirectoryBuilder += "/";
		}
	}
	return nullptr;
}

FString AWebServer::GetRegisteredHTMLResponse(TArray<FString> SubDirectories)
{
	FString SubDirectoryBuilder = "";
	for (int i = 0; i < SubDirectories.Num(); i++)
	{
		SubDirectoryBuilder += SubDirectories[i];
		if (URLHTMLResponses.Contains(SubDirectoryBuilder) &&
			(*SubURLCaptureAllowed.Find(SubDirectoryBuilder) == true || i == SubDirectories.Num() - 1))
		{
			return URLHTMLResponses[SubDirectoryBuilder];
		}
		else if (i != 0 && i != SubDirectories.Num() - 1)
		{
			SubDirectoryBuilder += "/";
		}
	}
	return "";
}