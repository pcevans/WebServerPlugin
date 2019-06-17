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

TArray<uint8> UDataConversionLibrary::ConvertStringToByteArray(FString String, bool bStripStringEnding)
{
	TArray<uint8> StringByteArray(String.GetCharArray());
	if (bStripStringEnding)
	{
		while (StringByteArray[StringByteArray.Num() - 1] == '\0')
			StringByteArray.RemoveAt(StringByteArray.Num() - 1);
	}
	return StringByteArray;
}

TArray<uint8> *UDataConversionLibrary::ConvertFileToByteArray(FString FullFilePath)
{
	TArray<uint8> *FileByteArray = new TArray<uint8>();
	FFileHelper::LoadFileToArray(*FileByteArray, *FullFilePath);
	//FileByteArray->RemoveAt(FileByteArray->Num() - 1);
	//FileByteArray->RemoveAt(FileByteArray.Num() - 1);
	return FileByteArray;
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