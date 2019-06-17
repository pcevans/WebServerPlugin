#include "HTTPMessageLibrary.h"

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
	TPairInitializer<const FString&, const FString&>("ogv","video/ogg"), TPairInitializer<const FString&, const FString&>("webm","video/webm"), TPairInitializer<const FString&, const FString&>("wmv","video/x-ms-wmv")
};

TMap<int32, FString> StatusCodeMap = {
	//100
	TPairInitializer<const int32&, const FString&>(100, "100 Continue"), TPairInitializer<const int32&, const FString&>(101, "101 Switching Protocols"), TPairInitializer<const int32&, const FString&>(102, "102 Processing"),/*WebDAV*/
	//200
	TPairInitializer<const int32&, const FString&>(200, "200 OK"), TPairInitializer<const int32&, const FString&>(201, "201 Created"), TPairInitializer<const int32&, const FString&>(202, "202 Accepted"),
	TPairInitializer<const int32&, const FString&>(203, "203 Non-Authoritative Information"), TPairInitializer<const int32&, const FString&>(204,"204 No Content"), TPairInitializer<const int32&, const FString&>(205, "205 Reset Content"),
	TPairInitializer<const int32&, const FString&>(206, "206 Partial Content"), TPairInitializer<const int32&, const FString&>(207, "207 Multi-Status"),/*WebDAV*/  TPairInitializer<const int32&, const FString&>(208, "208 Already Reported"),/*WebDAV*/
	TPairInitializer<const int32&, const FString&>(226, "226 IM Used"),
	//300
	TPairInitializer<const int32&, const FString&>(300, "300 Multiple Choices"), TPairInitializer<const int32&, const FString&>(301, "301 Moved Permanently"), TPairInitializer<const int32&, const FString&>(302, "302 Found"),
	TPairInitializer<const int32&, const FString&>(303, "303 See Other"), TPairInitializer<const int32&, const FString&>(304, "304 Not Modified"), TPairInitializer<const int32&, const FString&>(305, "305 Use Proxy"),
	TPairInitializer<const int32&, const FString&>(307, "307 Temporary Redirect"), TPairInitializer<const int32&, const FString&>(308, "308 Permanent Redirect"),
	//400
	TPairInitializer<const int32&, const FString&>(400, "400 Bad Request"), TPairInitializer<const int32&, const FString&>(401, "401 Unauthorized"), TPairInitializer<const int32&, const FString&>(402, "402 Payment Required"),
	TPairInitializer<const int32&, const FString&>(403, "403 Forbidden"), TPairInitializer<const int32&, const FString&>(404, "404 Not Found"), TPairInitializer<const int32&, const FString&>(405, "405 Method Not Allowed"),
	TPairInitializer<const int32&, const FString&>(406, "406 Not Acceptable"), TPairInitializer<const int32&, const FString&>(407, "407 Proxy Authentication Required"), TPairInitializer<const int32&, const FString&>(408, "408 Request Timeout"),
	TPairInitializer<const int32&, const FString&>(409, "409 Conflict"), TPairInitializer<const int32&, const FString&>(410, "410 Gone"), TPairInitializer<const int32&, const FString&>(411, "411 Length Required"),
	TPairInitializer<const int32&, const FString&>(412, "412 Precondition Failed"), TPairInitializer<const int32&, const FString&>(413, "413 Request Entity Too Large"), TPairInitializer<const int32&, const FString&>(414, "414 Request-URI Too Long"),
	TPairInitializer<const int32&, const FString&>(415, "415 Unsupported Media Type"), TPairInitializer<const int32&, const FString&>(416, "416 Requested Range Not Satisfiable"), TPairInitializer<const int32&, const FString&>(417, "417 Expectation Failed"),
	TPairInitializer<const int32&, const FString&>(418, "418 I'm a teapot"), TPairInitializer<const int32&, const FString&>(420, "420 Enhance Your Calm"),/*Twitter*/ TPairInitializer<const int32&, const FString&>(422, "422 Unprocessable Entity"),/*WebDAV*/
	TPairInitializer<const int32&, const FString&>(423, "423 Locked"),/*WebDAV*/ TPairInitializer<const int32&, const FString&>(424, "424 Failed Dependency"),/*WebDAV*/ TPairInitializer<const int32&, const FString&>(426, "426 Upgrade Required"),
	TPairInitializer<const int32&, const FString&>(428, "428 Precondition Required"), TPairInitializer<const int32&, const FString&>(429, "429 Too Many Requests"), TPairInitializer<const int32&, const FString&>(431, "431 Request Header Fields Too Large"),
	TPairInitializer<const int32&, const FString&>(444, "444 No Response"),/*Nginx*/ TPairInitializer<const int32&, const FString&>(449, "449 Retry With"),/*Microsoft*/ TPairInitializer<const int32&, const FString&>(450, "450 Blocked by Windows Parental Controls"),/*Microsoft*/
	TPairInitializer<const int32&, const FString&>(451, "451 Unavailable For Legal Reasons"), TPairInitializer<const int32&, const FString&>(499, "499 Client Closed Request"),/*Nginx*/
	//500
	TPairInitializer<const int32&, const FString&>(500, "500 Internal Server Error"), TPairInitializer<const int32&, const FString&>(501, "501 Not Implemented"), TPairInitializer<const int32&, const FString&>(502, "502 Bad Gateway"),
	TPairInitializer<const int32&, const FString&>(503, "503 Service Unavailable"), TPairInitializer<const int32&, const FString&>(504, "504 Gateway Timeout"), TPairInitializer<const int32&, const FString&>(505, "505 HTTP Version Not Supported"),
	TPairInitializer<const int32&, const FString&>(506, "506 Variant Also Negotiates"), TPairInitializer<const int32&, const FString&>(507, "507 Insufficient Storage"),/*WebDAV*/ TPairInitializer<const int32&, const FString&>(508, "508 Loop Detected"),/*WebDAV*/
	TPairInitializer<const int32&, const FString&>(509, "509 Bandwidth Limit Exceeded"),/*Apache*/ TPairInitializer<const int32&, const FString&>(510, "510 Not Extended"), TPairInitializer<const int32&, const FString&>(511, "511 Network Authentication Required"),
	TPairInitializer<const int32&, const FString&>(598, "598 Network read timeout error"), TPairInitializer<const int32&, const FString&>(599, "599 Network connect timeout error")
};

FSHTTPRequestDetails UHTTPMessageLibrary::ParseRawRequest(TArray<uint8> RequestBytes)
{
	FSHTTPRequestDetails ParsedRequestDetails;

	const FString ReceivedMessage = UDataConversionLibrary::ConvertByteArrayToString(RequestBytes);
	TArray<FString> RequestLines;
	ReceivedMessage.ParseIntoArray(RequestLines, TEXT("\n"), false);
	

	int LineNumber = 0;
	ParseRequestLine(RequestLines[LineNumber], ParsedRequestDetails);
	for (LineNumber = 1; RequestLines[LineNumber].Len() > 0; LineNumber++)
	{
		ParseRequestHeaderLine(RequestLines[LineNumber], ParsedRequestDetails);
	}
	LineNumber++;
	ParseRequestBody(RequestLines, ParsedRequestDetails, LineNumber);

	//*****************DEBUG**********************
	FString FullRequestDebugMessage = FString::Printf(TEXT("Request Message: \n%s"), *ReceivedMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FullRequestDebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FullRequestDebugMessage);

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

TArray<uint8> UHTTPMessageLibrary::BuildRawResponse(FSHTTPResponseDetails ParsedResponseDetails)
{
	FString ResponseMessage = "HTTP/1.1 ";
	FString CodeText = ConvertStatusCodeToString(ParsedResponseDetails.m_Code);
	ResponseMessage += CodeText;
	ResponseMessage += "\r\n";

	TArray<FString> HeaderKeys;
	ParsedResponseDetails.m_Headers.GetKeys(HeaderKeys);
	for (int i = 0; i < ParsedResponseDetails.m_Headers.Num(); i++)
	{
		FString HeaderValue = ParsedResponseDetails.m_Headers[HeaderKeys[i]];
		FString HeaderLine = HeaderKeys[i] + ": " + HeaderValue;
		ResponseMessage += HeaderLine;
		ResponseMessage += "\r\n";
	}
	ResponseMessage += "\r\n";

	TArray<uint8> ResponseBytes = UDataConversionLibrary::ConvertStringToByteArray(ResponseMessage, true);
	ResponseBytes.Append(ParsedResponseDetails.m_Body);

	//*****************DEBUG**********************
	FString FullResponseDebugMessage = FString::Printf(TEXT("Response Message:\n%s"), *ResponseMessage);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FullResponseDebugMessage);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FullResponseDebugMessage);

	UE_LOG(LogTemp, Warning, TEXT("Response details: code = %s"), *CodeText);
	UE_LOG(LogTemp, Warning, TEXT("Response details: Headers:"));
	for (int i = 0; i < ParsedResponseDetails.m_Headers.Num(); i++)
	{
		FString HeaderValue = ParsedResponseDetails.m_Headers[HeaderKeys[i]];
		FString HeaderLine = HeaderKeys[i] + ": " + HeaderValue;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *(HeaderLine));
	}
	FString BodyMessage = UDataConversionLibrary::ConvertByteArrayToString(ParsedResponseDetails.m_Body);
	ResponseMessage += BodyMessage;
	UE_LOG(LogTemp, Warning, TEXT("Response details: Body:\n%s"), *BodyMessage);
	//*****************END DEBUG******************

	return ResponseBytes;
}

void UHTTPMessageLibrary::ParseRequestLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)// FString &Verb, FString &RelativeURI, TMap<FString, FString> &QueryParameters, FString &BaseDirectory, TArray<FString> &SubDirectories)
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

void UHTTPMessageLibrary::ParseRequestHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails)
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

void UHTTPMessageLibrary::ParseRequestBody(TArray<FString> Lines, FSHTTPRequestDetails &ParsedRequestDetails, int32 StartLine)
{
	FString SingleLine = "";
	for (int i = 0; i < Lines.Num(); i++)
	{
		SingleLine += Lines[i] + "\n";
	}
	SingleLine.RemoveFromEnd("\n");
	ParsedRequestDetails.m_Body = UDataConversionLibrary::ConvertStringToByteArray(SingleLine, true);
}

FString UHTTPMessageLibrary::ConvertFileTypeToMIMEType(FString FileExtension)
{
	if (MIMETypeMap.Contains(FileExtension))
		return MIMETypeMap[FileExtension];
	else
		return MIMETypeMap["default"];
}


FString UHTTPMessageLibrary::ConvertStatusCodeToString(int32 StatusCode)
{
	if (StatusCodeMap.Contains(StatusCode))
		return StatusCodeMap[StatusCode];
	else
		return FString::Printf(TEXT("%d"), StatusCode);
}