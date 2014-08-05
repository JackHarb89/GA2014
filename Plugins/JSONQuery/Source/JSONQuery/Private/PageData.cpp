// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
#include "JSONQueryPrivatePCH.h"

//////////////////////////////////////////////////////////////////////////
// URequestHandler

/**
* Constructor
*/
UPageData::UPageData(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP) {
	Reset();
}

/**
* Grabs a page from the internets
*
* @param	WorldContextObject		The current context
* @param	url						The URL to request
*
* @return	A pointer to the newly created post data
*/
UPageData* UPageData::GetRequest(UObject* WorldContextObject, const FString &url) {
	// Create new page data for the response
	UPageData* pageObj = Create(WorldContextObject);

	// Create the HTTP request
	TSharedRef< IHttpRequest > HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(CreateURL(url));
	HttpRequest->OnProcessRequestComplete().BindUObject(pageObj, &UPageData::OnReady);
	
	// Execute the request
	HttpRequest->ProcessRequest();

	// Return the page data
	return pageObj;
}

/**
* Create a new instance of the UPageData class, for use in Blueprint graphs.
*
* @param	WorldContextObject		The current context
*
* @return	A pointer to the newly created post data
*/
UPageData* UPageData::Create(UObject* WorldContextObject) {
	// Get the world object from the context
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);

	// Construct the object and return it
	return (UPageData*)StaticConstructObject(UPageData::StaticClass());
}

/**
* Prefixes the input URL with http:// if necessary
*
* @param	inputURL		The input URL
*
* @return	The output URL
*/
FString UPageData::CreateURL(FString inputURL) {
	if (!inputURL.StartsWith("http://")) {
		return "http://" + inputURL;
	}

	return inputURL;
}

/**
* Posts the current request data to the internet
*
* @param	WorldContextObject		The current context
* @param	url						The URL to post to
*
*/
void UPageData::PostRequest(UObject* WorldContextObject, const FString &url) {
	FString outStr;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&outStr);
	
	// Start writing the response
	JsonWriter->WriteObjectStart();

	// Loop through all the values in the post data
	for (auto RequestIt = Data->Values.CreateIterator(); RequestIt; ++RequestIt) {
		FString key = RequestIt.Key();
		FString value = RequestIt.Value().Get()->AsString();

		// Write entry
		JsonWriter->WriteValue(key,value);
	}

	// Stop writing and close the writer
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	// Log the post data for the user (OPTIONAL)
	//UE_LOG(LogTemp, Warning, TEXT("Post data: %s"), *outStr);

	// Create the post request with the generated data
	TSharedRef< IHttpRequest > HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL(CreateURL(url));
	HttpRequest->SetContentAsString(outStr);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UPageData::OnReady);

	// Execute the request
	HttpRequest->ProcessRequest();
}

/**
* Adds the supplied key and value to the post data
*
* @param	key						Object key
* @param	value					Object value
*
*/
void UPageData::AddData(const FString& key, const FString& value) {
	Valid = true;

	// Add the key and value to the post data
	Data->SetStringField(*key,*value);
}

/**
* Gets the JSON data from the current post data
*
* @param	Key						The key to fetch from the post data
*
* @return	The value associated with the key
*/
FString UPageData::GetData(const FString& key) {
	// If the current post data isn't valid, return an empty string
	if (Valid) {
		TSharedPtr<FJsonValue> value = Data->TryGetField(key);
		
		// Only return if the value is valid
		if (value.IsValid()) {
			return value->AsString();
		}
	}

	// If the value could not be retrieved, return an empty string
	return "";
}

/**
* Resets the current page data
*
*/
void UPageData::Reset() {
	// If the post data is valid
	if (Data.IsValid()) {
		// Clear the current post data
		Data.Reset();
	}

	// Create a new JSON object
	Data = MakeShareable(new FJsonObject());
	Valid = false;
}

/**
* Callback for IHttpRequest::OnProcessRequestComplete()
*
* @param	Request					HTTP request pointer
* @param	Response				Response pointer
* @param	bWasSuccessful			Whether the request was successful or not
*
*/
void UPageData::OnReady(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful) {
		Valid = false;
		UE_LOG(LogTemp, Error, TEXT("Response was invalid! Please check the URL."));
		return;
	}

	// Log the response
	//UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *Response->GetContentAsString());

	// Initialize the JSON reader with the supplied content
	FString responseData = Response->GetContentAsString();
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(responseData);

	// Deserialize the JSON data
	FJsonSerializer::Deserialize(JsonReader, Data);

	// Decide whether the request was successful
	Valid = Data.IsValid() && bWasSuccessful;

	// Log errors if necessary
	if (!Valid) {
		if (!bWasSuccessful) {
			UE_LOG(LogJson, Error, TEXT("Request failed!"));
		} else if (!Data.IsValid()) {
			UE_LOG(LogJson, Error, TEXT("JSON could not be decoded! Supplied data:\n%s"), *Response->GetContentAsString());
		}
	}

	// Assign the request content
	Content = Response->GetContentAsString();

	// Broadcast the result event
	OnGetResult.Broadcast();
}