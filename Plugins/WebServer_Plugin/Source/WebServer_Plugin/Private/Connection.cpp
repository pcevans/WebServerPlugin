// Fill out your copyright notice in the Description page of Project Settings.


#include "Connection.h"

/*void UConnection::RegisterHTMLResponses()
{
	RegisteredHTMLResponses.Add("/", "<!DOCTYPE html><html><body><h1>My First Web Server</h1><p>You have been served.</p></body></html>");
	RegisteredHTMLResponses.Add("/callback", "<!DOCTYPE html><html><body><h1>Callback</h1><p>Code is {{code}}</p></body></html>");
}*/

void UConnection::Initialize(FSocket *Socket, TArray<uint8> ReceivedData)
{
	ConnectionSocket = Socket;
	ParseRequestDetails(ReceivedData);
}

void UConnection::SendResponse()
{
	CreateAndSendMessage();
}

void UConnection::SendResponseWithDetails(FSHTTPResponseDetails Details)
{
	ResponseDetails = Details;

	CreateAndSendMessage();
}

void UConnection::SendSimpleHTMLResponse(int32 ResponseCode, FString HTML)
{
	ResponseDetails.m_Code = ResponseCode;
	ResponseDetails.m_Headers.Add("Content-Type", "text/html");
	ResponseDetails.m_Body = UDataConversionLibrary::ConvertStringToByteArray(HTML);

	CreateAndSendMessage();
}

void UConnection::CreateAndSendMessage()
{
	FString ResponseMessage = CreateResponseMessage();
	TArray<uint8> ResponseByteArray = UDataConversionLibrary::ConvertStringToByteArray(ResponseMessage);
	int32 BytesSent;
	ConnectionSocket->Send(ResponseByteArray.GetData(), ResponseByteArray.Num(), BytesSent);
	ConnectionSocket->Close();
}

FString UConnection::CreateResponseMessage() 
{
	FString ResponseMessage = "HTTP/1.1 ";
	FString CodeText = UDataConversionLibrary::ConvertHTTPStatusCodeToString(ResponseDetails.m_Code);
	ResponseMessage += CodeText;
	UE_LOG(LogTemp, Warning, TEXT("Response details: code = %s"), *CodeText);
	ResponseMessage += "\n";

	TArray<FString> HeaderKeys;
	ResponseDetails.m_Headers.GetKeys(HeaderKeys);
	UE_LOG(LogTemp,Warning,TEXT("Response details: Headers:"))
	for (int i = 0; i < ResponseDetails.m_Headers.Num(); i++)
	{
		FString HeaderValue = ResponseDetails.m_Headers[HeaderKeys[i]];
		FString HeaderLine = HeaderKeys[i] + ": " + HeaderValue;
		ResponseMessage += HeaderLine;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *(HeaderLine));
		ResponseMessage += "\n";
	}
	ResponseMessage += "\n";
	FString BodyMessage = UDataConversionLibrary::ConvertByteArrayToString(ResponseDetails.m_Body);
	ResponseMessage += BodyMessage;
	UE_LOG(LogTemp, Warning, TEXT("Response details: Body:\n%s"), *BodyMessage);

	FString FullResponseDebugMessage = FString::Printf(TEXT("Response Message:\n%s"), *ResponseMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FullResponseDebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FullResponseDebugMessage);
	return ResponseMessage;
}

void UConnection::ParseRequestDetails(TArray<uint8> ReceivedData)
{
	FSHTTPRequestDetails ParsedRequestDetails;

	const FString ReceivedMessage = UDataConversionLibrary::ConvertByteArrayToString(ReceivedData);
	TArray<FString> RequestLines;
	ReceivedMessage.ParseIntoArray(RequestLines, TEXT("\n"), false);
	FString FullRequestDebugMessage = FString::Printf(TEXT("Request Message: \n%s"), *ReceivedMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FullRequestDebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FullRequestDebugMessage);

	int LineNumber = 0;
	ParseRequestLine(RequestLines[LineNumber], ParsedRequestDetails);
	for (LineNumber = 1; RequestLines[LineNumber].Len() > 0; LineNumber++)
	{
		ParseHeaderLine(RequestLines[LineNumber], ParsedRequestDetails);
	}
	LineNumber++;
	ParseBody(RequestLines, RequestDetails, LineNumber);

	UE_LOG(LogTemp, Warning, TEXT("Request details: verb %s; file url %s; host %s"), *ParsedRequestDetails.m_Verb, *ParsedRequestDetails.m_FileURL, *ParsedRequestDetails.m_Host);
	TArray<FString> HeaderKeys;
	ParsedRequestDetails.m_Headers.GetKeys(HeaderKeys);
	UE_LOG(LogTemp, Warning, TEXT("Request details: Headers: "));
	for (int i = 0; i < ParsedRequestDetails.m_Headers.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *HeaderKeys[i], *ParsedRequestDetails.m_Headers[HeaderKeys[i]]);
	}
	TArray<FString> QueryKeys;
	ParsedRequestDetails.m_QueryParameters.GetKeys(QueryKeys);
	UE_LOG(LogTemp, Warning, TEXT("Request details: Query parameters: "));
	for (int i = 0; i < ParsedRequestDetails.m_QueryParameters.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *QueryKeys[i], *ParsedRequestDetails.m_QueryParameters[QueryKeys[i]]);
	}
	UE_LOG(LogTemp, Warning, TEXT("Request details: Subdirectories:"));
	for (int i = 0; i < ParsedRequestDetails.m_SubDirectories.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subdirectory %d: %s"), i, *ParsedRequestDetails.m_SubDirectories[i]);
	}


	RequestDetails = ParsedRequestDetails;
}

void UConnection::ParseRequestLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)// FString &Verb, FString &RelativeURI, TMap<FString, FString> &QueryParameters, FString &BaseDirectory, TArray<FString> &SubDirectories)
{
	std::string RequestString = std::string(TCHAR_TO_UTF8(*Line));
	std::regex RequestRegex("^([a-zA-Z]*) (/[^ ]*)");
	std::smatch RequestCaptureGroups;
	std::string WebPagePath;
	if (std::regex_search(RequestString, RequestCaptureGroups, RequestRegex))
	{
		ParsedRequestDetails.m_Verb = RequestCaptureGroups[1].str().c_str();
		WebPagePath = RequestCaptureGroups[2].str();
		ParsedRequestDetails.m_FileURL = WebPagePath.c_str();

		UE_LOG(LogTemp, Warning, TEXT("Relative page path (with query): %s"), *FString(WebPagePath.c_str()));
	}

	std::regex QueryRegex("[?&]([^ =&]*)(=([^ &]*))?");
	std::smatch QueryCaptureGroups;
	std::string QueryString = WebPagePath;
	while (std::regex_search(QueryString, QueryCaptureGroups, QueryRegex))
	{
		if (QueryString.compare(WebPagePath) == 0)
		{
			WebPagePath = QueryCaptureGroups.prefix();
			ParsedRequestDetails.m_FileURL = WebPagePath.c_str();

			UE_LOG(LogTemp, Warning, TEXT("Relative page path: %s"), *ParsedRequestDetails.m_FileURL);
		}

		std::string QueryKey = QueryCaptureGroups[1].str();
		std::string QueryValue = QueryCaptureGroups[3].str();
		ParsedRequestDetails.m_QueryParameters.Add(QueryKey.c_str(), QueryValue.c_str());

		QueryString = QueryCaptureGroups.suffix();
	}

	std::regex DirectoryRegex("([^/]*/?)");
	std::smatch DirectoryCaptureGroups;
	std::string DirectoryString = WebPagePath;
	
	while (DirectoryString != "" && std::regex_search(DirectoryString, DirectoryCaptureGroups, DirectoryRegex))
	{
		FString SubDirectory = DirectoryCaptureGroups[1].str().c_str();
		if (DirectoryString.compare(WebPagePath) == 0)
		{
			ParsedRequestDetails.m_BaseDirectory = SubDirectory;
		}
		else
		{
			SubDirectory.RemoveFromEnd("/");
		}
		ParsedRequestDetails.m_SubDirectories.Add(SubDirectory);
		DirectoryString = DirectoryCaptureGroups.suffix();
	}
}

void UConnection::ParseHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)
{
	std::regex HeaderRegex("([^:]*): ([^ \r]*)");
	std::smatch HeaderCaptureGroups;
	std::string HeaderString = TCHAR_TO_UTF8(*Line);
	if (std::regex_search(HeaderString, HeaderCaptureGroups, HeaderRegex))
	{
		std::string HeaderKey = HeaderCaptureGroups[1].str();
		std::string HeaderValue = HeaderCaptureGroups[2].str();
		if (HeaderKey.compare("Host") == 0)
		{
			ParsedRequestDetails.m_Host = HeaderValue.c_str();
		}
		ParsedRequestDetails.m_Headers.Add(HeaderKey.c_str(), HeaderValue.c_str());
	}
}

void UConnection::ParseBody(TArray<FString> Lines, FSHTTPRequestDetails &ParsedRequestDetails, int32 StartLine)
{
	FString SingleLine = "";
	for (int i = 0; i < Lines.Num(); i++)
	{
		SingleLine += Lines[i] + "\n";
	}
	SingleLine.RemoveFromEnd("\n");
	ParsedRequestDetails.m_Body = UDataConversionLibrary::ConvertStringToByteArray(SingleLine);
}