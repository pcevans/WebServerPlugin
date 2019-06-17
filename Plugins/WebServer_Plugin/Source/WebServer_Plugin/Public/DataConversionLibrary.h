#pragma once

#include <string>
#include <regex>

#include "CoreMinimal.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DataConversionLibrary.generated.h"

enum EJsonTypes {
	None,
	Object,
	Attribute,
	Array
};

USTRUCT(BlueprintType)
struct FSDataObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FString Value;
	TMap<FString, TArray<FSDataObject>> Attributes;
};

/**
 * 
 */
UCLASS()
class WEBSERVER_PLUGIN_API UDataConversionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FString ConvertByteArrayToString(TArray<uint8> ByteArray);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static TArray<uint8> ConvertStringToByteArray(FString String, bool bStripStringEnding=true);
	//UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static TArray<uint8> *ConvertFileToByteArray(FString FullFilePath);
	//UFUNCTION(BlueprintCallable, Category = "Data Conversion")
	//	static FSDataObject ConvertJSONToDataObject(FString JSON);
	//UFUNCTION(BlueprintCallable, Category = "Data Conversion")
	//	static FSDataObject ConvertXMLToDataObject(FString XML);

private:
	//For JSON to struct
	static void ParseJsonInternal(FSDataObject &DataObject, std::string JSON, EJsonTypes LastType);

};
