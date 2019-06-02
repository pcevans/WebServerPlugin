// Fill out your copyright notice in the Description page of Project Settings.


#include "DataConversionLibrary.h"

FString UDataConversionLibrary::ConvertByteArrayToString(TArray<uint8> ByteArray)
{
	ByteArray.Add(0); // Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ByteArray.GetData())));
}

TArray<uint8> UDataConversionLibrary::ConvertStringToByteArray(FString String)
{
	TArray<uint8> StringByteArray(String.GetCharArray());
	return StringByteArray;
}

TArray<uint8> UDataConversionLibrary::ConvertFileToByteArray(FString FullFilePath)
{
	TArray<uint8> FileByteArray;
	FFileHelper::LoadFileToArray(FileByteArray, *FullFilePath);
	return FileByteArray;
}

TMap<FString, FString> MIMETypeMap = {
	//default
	TPairInitializer<const FString&, const FString&>("default","application/octet-stream"),
	//application
	TPairInitializer<const FString&, const FString&>("7z","application/x-7z-compressed"), TPairInitializer<const FString&, const FString&>("apk","application/vnd.android.package-archive"),
	TPairInitializer<const FString&, const FString&>("jar", "application/java-archive"),
	TPairInitializer<const FString&, const FString&>("js", "application/javascript"), TPairInitializer<const FString&, const FString&>("json", "application/json"),
	TPairInitializer<const FString&, const FString&>("exe", "application/x-msdownload"),
	TPairInitializer<const FString&, const FString&>("pdf","application/pdf"), TPairInitializer<const FString&, const FString&>("psd","application/photoshop"),
	TPairInitializer<const FString&, const FString&>("rar","application/x-rar-compressed"),
	TPairInitializer<const FString&, const FString&>("swf","application/x-shockwave-flash"),
	TPairInitializer<const FString&, const FString&>("tar", "application/x-tar"), TPairInitializer<const FString&, const FString&>("torrent", "application/x-bittorent"),
	TPairInitializer<const FString&, const FString&>("xml","application/xml"), TPairInitializer<const FString&, const FString&>("xhtml","application/xhtml+xml"),
	TPairInitializer<const FString&, const FString&>("zip","application/zip"),
	//application - Microsoft Office
	TPairInitializer<const FString&, const FString&>("doc","application/msword"), TPairInitializer<const FString&, const FString&>("ppt","application/vnd.ms-powerpoint"), TPairInitializer<const FString&, const FString&>("xls","application/vnd.ms-excel"),
	TPairInitializer<const FString&, const FString&>("docx","application/vnd.openxmlformats-officedocument.wordprocessingml.document"),
	TPairInitializer<const FString&, const FString&>("pptx","application/vnd.openxmlformats-officedocument.presentationml.presentation"),
	TPairInitializer<const FString&, const FString&>("xlsx","application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"),
	//audio
	TPairInitializer<const FString&, const FString&>("aac","audio/aac"), TPairInitializer<const FString&, const FString&>("mp3","audio/mpeg"), TPairInitializer<const FString&, const FString&>("ogg","audio/ogg"), 
	TPairInitializer<const FString&, const FString&>("wav","audio/wav"), TPairInitializer<const FString&, const FString&>("weba","audio/webm"), TPairInitializer<const FString&, const FString&>("wma","audio/x-ms-wma"),
	//image
	TPairInitializer<const FString&, const FString&>("bmp","image/bmp"), TPairInitializer<const FString&, const FString&>("gif", "image/gif"), TPairInitializer<const FString&, const FString&>("ico", "image/x-icon"), 
	TPairInitializer<const FString&, const FString&>("jpg","image/jpeg"), TPairInitializer<const FString&, const FString&>("jpeg","image/jpeg"), TPairInitializer<const FString&, const FString&>("png", "image/png"), 
	TPairInitializer<const FString&, const FString&>("tiff","image/tiff"), 
	//text
	TPairInitializer<const FString&, const FString&>("css", "text/css"), TPairInitializer<const FString&, const FString&>("csv", "text/csv"), TPairInitializer<const FString&, const FString&>("htm","text/html"), 
	TPairInitializer<const FString&, const FString&>("html", "text/html"), TPairInitializer<const FString&, const FString&>("rtf","text/rtf"), TPairInitializer<const FString&, const FString&>("txt","text/plain"),
	//video
	TPairInitializer<const FString&, const FString&>("avi","video/x-msvideo"),TPairInitializer<const FString&, const FString&>("flv","video/x-flv"), TPairInitializer<const FString&, const FString&>("mid","audio/midi"), TPairInitializer<const FString&, const FString&>("mp4","video/mp4"), TPairInitializer<const FString&, const FString&>("mpeg","video/mpeg"),
	TPairInitializer<const FString&, const FString&>("ogv","video/ogg"), TPairInitializer<const FString&, const FString&>("webm","video/webm"), TPairInitializer<const FString&, const FString&>("wmv","video/x-ms-wmv"),
 };


FString UDataConversionLibrary::ConvertFileTypeToMIMEType(FString FileExtension)
{
	if (MIMETypeMap.Contains(FileExtension))
		return MIMETypeMap[FileExtension];
	else
		return MIMETypeMap["default"];
}

/*FSDataObject UDataConversionLibrary::ConvertJSONToDataObject(FString JSON)
{
	FSDataObject DataObject;
	std::string JsonString = TCHAR_TO_UTF8(*JSON);

	ParseJsonInternal(DataObject, JsonString, EJsonTypes::None);

	return DataObject;
}*/

void UDataConversionLibrary::ParseJsonInternal(FSDataObject &DataObject, std::string JSON, EJsonTypes LastType)
{

	const static std::regex ObjectRegex("{(.*)}");
	const static std::regex AttributeRegex("\"([^\"]*)\":?(.*)");
	const static std::regex ArrayRegex("\\[([^]]*)]");

	if (JSON.length() == 0)
		return;

	std::smatch JsonCaptureGroups;
	std::string CurrentJson = JSON;
	switch (LastType)
	{
	case EJsonTypes::Object:
		//find all attributes in object
		if (std::regex_search(CurrentJson, JsonCaptureGroups, AttributeRegex)) 
		{
			do
			{
				FString AttributeName = JsonCaptureGroups[1].str().c_str();
				std::string InternalJson = JsonCaptureGroups[2];

				TArray<FSDataObject> ObjectList = DataObject.Attributes.FindOrAdd(AttributeName);
				FSDataObject NewDataObject;
				ObjectList.Add(NewDataObject);
				
				ParseJsonInternal(NewDataObject, InternalJson, EJsonTypes::Attribute);

				CurrentJson = JsonCaptureGroups.suffix();
			} while (std::regex_search(CurrentJson, JsonCaptureGroups, AttributeRegex));
			if (CurrentJson.length() > 0)
			{
				FString JsonString = JSON.c_str();
				FString CurrentJsonString = CurrentJson.c_str();
				UE_LOG(LogTemp, Warning, TEXT("Warning: Some attributes parsed but full Json parse failed! Internal Json for object %s: %s\nCurrent Json leftovers: %s"), *DataObject.Value, *JsonString, *CurrentJsonString);
			}
		}
		else if (std::regex_match(CurrentJson, JsonCaptureGroups, ArrayRegex))
		{
			std::string InternalJson = JsonCaptureGroups[1];
			ParseJsonInternal(DataObject, InternalJson, EJsonTypes::Array);
		}
		else
		{
			FString JsonString = JSON.c_str();
			UE_LOG(LogTemp, Warning, TEXT("Warning: Json parse totally failed! Internal Json for object %s: %s"),*DataObject.Value, *JsonString);
		}
		break;
	case EJsonTypes::Array:
		//determine whether object or single-value array
		//determine whether from object or from attribute for parent attribute naming
		break;
	case EJsonTypes::Attribute:
		//determine whether object, array, or single-value attribute
		break;
	default:
		DataObject.Value = "object";
		if (std::regex_match(JSON, JsonCaptureGroups, ObjectRegex))
		{
			std::string InternalJson = JsonCaptureGroups[1];
			ParseJsonInternal(DataObject, InternalJson, EJsonTypes::Object);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning: Failed to parse JSON - base layer is not an object!"))
		}
	}
}

FString UDataConversionLibrary::ConvertHTTPStatusCodeToString(int32 StatusCode)
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

FSHTTPRequestDetails UDataConversionLibrary::ConvertHTTPRequestBytesToDetailStruct(TArray<uint8> RequestBytes)
{
	FSHTTPRequestDetails ParsedRequestDetails;

	const FString ReceivedMessage = UDataConversionLibrary::ConvertByteArrayToString(RequestBytes);
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
	ParseBody(RequestLines, ParsedRequestDetails, LineNumber);

	//*****************DEBUG**********************
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
	//******************END DEBUG******************

	return ParsedRequestDetails;
}

TArray<uint8> UDataConversionLibrary::ConvertHTTPResponseDetailStructToBytes(FSHTTPResponseDetails ParsedResponseDetails)
{
	FString ResponseMessage = "HTTP/1.1 ";
	FString CodeText = UDataConversionLibrary::ConvertHTTPStatusCodeToString(ParsedResponseDetails.m_Code);
	ResponseMessage += CodeText;
	UE_LOG(LogTemp, Warning, TEXT("Response details: code = %s"), *CodeText);
	ResponseMessage += "\n";

	TArray<FString> HeaderKeys;
	ParsedResponseDetails.m_Headers.GetKeys(HeaderKeys);
	UE_LOG(LogTemp, Warning, TEXT("Response details: Headers:"))
		for (int i = 0; i < ParsedResponseDetails.m_Headers.Num(); i++)
		{
			FString HeaderValue = ParsedResponseDetails.m_Headers[HeaderKeys[i]];
			FString HeaderLine = HeaderKeys[i] + ": " + HeaderValue;
			ResponseMessage += HeaderLine;
			UE_LOG(LogTemp, Warning, TEXT("%s"), *(HeaderLine));
			ResponseMessage += "\n";
		}
	ResponseMessage += "\n";

	TArray<uint8>ResponseBytes = ConvertStringToByteArray(ResponseMessage);
	ResponseBytes.Append(ParsedResponseDetails.m_Body);

	FString BodyMessage = UDataConversionLibrary::ConvertByteArrayToString(ParsedResponseDetails.m_Body);
	ResponseMessage += BodyMessage;
	UE_LOG(LogTemp, Warning, TEXT("Response details: Body:\n%s"), *BodyMessage);

	FString FullResponseDebugMessage = FString::Printf(TEXT("Response Message:\n%s"), *ResponseMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FullResponseDebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FullResponseDebugMessage);
	return ResponseBytes;
}

void UDataConversionLibrary::ParseRequestLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)// FString &Verb, FString &RelativeURI, TMap<FString, FString> &QueryParameters, FString &BaseDirectory, TArray<FString> &SubDirectories)
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

void UDataConversionLibrary::ParseHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)
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

void UDataConversionLibrary::ParseBody(TArray<FString> Lines, FSHTTPRequestDetails &ParsedRequestDetails, int32 StartLine)
{
	FString SingleLine = "";
	for (int i = 0; i < Lines.Num(); i++)
	{
		SingleLine += Lines[i] + "\n";
	}
	SingleLine.RemoveFromEnd("\n");
	ParsedRequestDetails.m_Body = UDataConversionLibrary::ConvertStringToByteArray(SingleLine);
}