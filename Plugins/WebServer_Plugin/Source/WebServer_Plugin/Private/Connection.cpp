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

}

void UConnection::SendResponseWithDetails(FSHTTPResponseDetails Details)
{
}

void UConnection::SendSimpleHTMLResponse(int32 ResponseCode, FString HTML)
{
	ResponseDetails.m_Code = ResponseCode;
	ResponseDetails.m_Headers.Add("Content-Type", "text/html");
	ResponseDetails.m_Body = ConvertStringToByteArray(HTML);

	CreateAndSendMessage();
}

void UConnection::CreateAndSendMessage()
{
	FString ResponseMessage = CreateResponseMessage();
	TArray<uint8> ResponseByteArray = ConvertStringToByteArray(ResponseMessage);
	int32 BytesSent;
	ConnectionSocket->Send(ResponseByteArray.GetData(), ResponseByteArray.Num(), BytesSent);
	ConnectionSocket->Close();
}

FString UConnection::CreateResponseMessage() 
{
	FString ResponseMessage = "HTTP/1.1 ";
	ResponseMessage += ConvertStatusCodeToString(ResponseDetails.m_Code);
	ResponseMessage += "\n";
	TArray<FString> HeaderKeys;
	ResponseDetails.m_Headers.GetKeys(HeaderKeys);
	for (int i = 0; i < ResponseDetails.m_Headers.Num(); i++)
	{
		FString HeaderValue = ResponseDetails.m_Headers[HeaderKeys[i]];
		ResponseMessage += HeaderKeys[i] + ": " + HeaderValue;
		ResponseMessage += "\n";
	}
	ResponseMessage += "\n";
	ResponseMessage += ConvertByteArrayToString(ResponseDetails.m_Body);

	UE_LOG(LogTemp, Warning, TEXT("Response Message:\n%s"), *ResponseMessage);
	return ResponseMessage;
}

FString UConnection::ConvertStatusCodeToString(int32 StatusCode) 
{
	if (StatusCode < 200)
	{
		switch (StatusCode)
		{
		case 100:
			return "100 Continue";
		case 101:
			return "101 Switching Protocols";
		case 102:
			return "102 Processing"; //WebDAV
		default:
			return FString::Printf(TEXT("%d"), StatusCode);
		}
	}
	else if (StatusCode < 300)
	{
		switch (StatusCode)
		{
		case 200:
			return "200 OK";
		case 201:
			return "201 Created";
		case 202:
			return "202 Accepted";
		case 203:
			return "203 Non-Authoritative Information";
		case 204:
			return "204 No Content";
		case 205:
			return "205 Reset Content";
		case 206:
			return "206 Partial Content";
		case 207:
			return "207 Multi-Status"; //WebDAV
		case 208:
			return "208 Already Reported"; //WebDAV
		case 226:
			return "226 IM Used";
		default:
			return FString::Printf(TEXT("%d"), StatusCode);
		}
	}
	else if (StatusCode < 400)
	{
		switch (StatusCode)
		{
		case 300:
			return "300 Multiple Choices";
		case 301:
			return "301 Moved Permanently";
		case 302:
			return "302 Found";
		case 303:
			return "303 See Other";
		case 304:
			return "304 Not Modified";
		case 305:
			return "305 Use Proxy";
		case 307:
			return "307 Temporary Redirect";
		case 308:
			return "308 Permanent Redirect";
		default:
			return FString::Printf(TEXT("%d"), StatusCode);
		}
	}
	else if (StatusCode < 500)
	{
		switch (StatusCode)
		{
		case 400:
			return "400 Bad Request";
		case 401:
			return "401 Unauthorized";
		case 402:
			return "402 Payment Required";
		case 403:
			return "403 Forbidden";
		case 404:
			return "404 Not Found";
		case 405:
			return "405 Method Not Allowed";
		case 406:
			return "406 Not Acceptable";
		case 407:
			return "407 Proxy Authentication Required";
		case 408:
			return "408 Request Timeout";
		case 409:
			return "409 Conflict";
		case 410:
			return "410 Gone";
		case 411:
			return "411 Length Required";
		case 412:
			return "412 Precondition Failed";
		case 413:
			return "413 Request Entity Too Large";
		case 414:
			return "414 Request-URI Too Long";
		case 415:
			return "415 Unsupported Media Type";
		case 416:
			return "416 Requested Range Not Satisfiable";
		case 417:
			return "417 Expectation Failed";
		case 418:
			return "418 I'm a teapot";
		case 420:
			return "420 Enhance Your Calm"; //Twitter
		case 422:
			return "422 Unprocessable Entity"; //WebDAV
		case 423:
			return "423 Locked"; //WebDAV
		case 424:
			return "424 Failed Dependency"; //WebDAV
		case 426:
			return "426 Upgrade Required";
		case 428:
			return "428 Precondition Required";
		case 429:
			return "429 Too Many Requests";
		case 431:
			return "431 Request Header Fields Too Large";
		case 444:
			return "444 No Response"; //Nginx
		case 449:
			return "449 Retry With"; //Microsoft
		case 450:
			return "450 Blocked by Windows Parental Controls"; //Microsoft
		case 451:
			return "451 Unavailable For Legal Reasons";
		case 499:
			return "499 Client Closed Request"; //Nginx
		default:
			return FString::Printf(TEXT("%d"), StatusCode);
		}
	}
	else
	{
		switch (StatusCode)
		{
		case 500:
			return "500 Internal Server Error";
		case 501:
			return "501 Not Implemented";
		case 502:
			return "502 Bad Gateway";
		case 503:
			return "503 Service Unavailable";
		case 504:
			return "504 Gateway Timeout";
		case 505:
			return "505 HTTP Version Not Supported";
		case 506:
			return "506 Variant Also Negotiates";
		case 507:
			return "507 Insufficient Storage"; //WebDAV
		case 508:
			return "508 Loop Detected"; //WebDAV
		case 509:
			return "509 Bandwidth Limit Exceeded"; //Apache
		case 510:
			return "510 Not Extended";
		case 511:
			return "511 Network Authentication Required";
		case 598:
			return "598 Network read timeout error";
		case 599:
			return "599 Network connect timeout error";
		default:
			return FString::Printf(TEXT("%d"), StatusCode);
		}
	}
}

FString UConnection::ConvertByteArrayToString(TArray<uint8> ByteArray)
{
	ByteArray.Add(0); // Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ByteArray.GetData())));
}

TArray<uint8> UConnection::ConvertStringToByteArray(FString String)
{
	TArray<uint8> StringByteArray;
	StringByteArray.SetNumUninitialized(String.Len() + 1);
	StringToBytes(String, StringByteArray.GetData(), String.Len() + 1);
	for (int i = 0; i < String.Len(); i++)
	{
		StringByteArray[i]++;
	}
	StringByteArray[String.Len()] = 0;

	return StringByteArray;
}

void UConnection::ParseRequestDetails(TArray<uint8> ReceivedData)
{
	FSHTTPRequestDetails ParsedRequestDetails;

	const FString ReceivedMessage = ConvertByteArrayToString(ReceivedData);
	TArray<FString> RequestLines;
	ReceivedMessage.ParseIntoArray(RequestLines, TEXT("\n"), false);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedMessage));

	int LineNumber = 0;
	ParseRequestLine(RequestLines[LineNumber], ParsedRequestDetails);// ParsedRequestDetails.m_Verb, ParsedRequestDetails.m_FileURL, ParsedRequestDetails.m_QueryParameters, ParsedRequestDetails.m_BaseDirectory, RequestDetails.m_SubDirectories);
	for (LineNumber = 1; RequestLines[LineNumber].Len() > 0; LineNumber++)
	{
		ParseHeaderLine(RequestLines[LineNumber], ParsedRequestDetails);// .m_Host, ParsedRequestDetails.m_Headers);
	}
	LineNumber++;
	//ParseBody(RequestLines, LineNumber, RequestDetails.m_Body);

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

	std::regex DirectoryRegex("(/[^/]*)");
	std::smatch DirectoryCaptureGroups;
	std::string DirectoryString = WebPagePath;
	while (std::regex_search(DirectoryString, DirectoryCaptureGroups, DirectoryRegex))
	{
		if (DirectoryString.compare(WebPagePath) == 0)
		{
			ParsedRequestDetails.m_BaseDirectory = DirectoryCaptureGroups[1].str().c_str();
		}
		FString SubDirectory = DirectoryCaptureGroups[1].str().c_str();
		ParsedRequestDetails.m_SubDirectories.Add(SubDirectory);
		DirectoryString = DirectoryCaptureGroups.suffix();
	}
}

void UConnection::ParseHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)//FString &Host, TMap<FString, FString> &HeaderMap)
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


/*FString UConnection::GetHTMLResponse(FSHTTPRequestDetails RequestDetails, bool &bResponseExists)
{
	FString *HTMLResponse = RegisteredHTMLResponses.Find(RequestDetails.m_FileURL);

	if (HTMLResponse != nullptr)
	{
		bResponseExists = true;
		return *HTMLResponse;
	}
	else
	{
		HTMLResponse = RegisteredHTMLResponses.Find(RequestDetails.m_BaseDirectory);
		if (HTMLResponse != nullptr)
		{
			bResponseExists = true;
			return *HTMLResponse;
		}
		else
		{
			bResponseExists = false;
			return "<!DOCTYPE html><html><body><h1>404</h1><p>No page exists at " + RequestDetails.m_FileURL + " or " + RequestDetails.m_BaseDirectory + ".</p></body></html>";
		}
	}
}*/

//void AWebServer::ServeWebPage(FString HTMLString, FString Code, )

void UConnection::ServeWebPage()
{
	bool bResponseExists = false;
	//FString Response = GetHTMLResponse(RequestDetails, bResponseExists);

	FString SendMessage;
	if (bResponseExists)
		SendMessage = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
	else
		SendMessage = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n";
	//SendMessage += Response;
	uint8* SendByteArray = ConvertStringToByteArray(SendMessage).GetData();	//new uint8[SendMessage.Len() + 1];
	
	int32 BytesSent;

	ConnectionSocket->Send(SendByteArray, SendMessage.Len() + 1, BytesSent);
}