// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "PageData.generated.h"

// Generate a delegate for the OnGetResult event
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGetResult);

UCLASS(BlueprintType, Blueprintable)
class UPageData : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	/* Internal bind method for the IHTTPRequest::OnProcessRequestCompleted() event */
	void OnReady(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/* Resets the current post data */
	void Reset();

	/* Prefixes the input URL with http:// if needed */
	static FString CreateURL(FString inputURL);
public:
	/* The actual post data */
	TSharedPtr<FJsonObject> Data;

	/* Contains the actual page content, as a string */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JSON")
	FString Content;

	/* Event which triggers when the content has been retrieved */
	UPROPERTY(BlueprintAssignable, Category = "JSON")
	FOnGetResult OnGetResult;

	/* Decides whether the current post data is valid or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "JSON")
	bool Valid;

	/* Adds JSON data to the post data */
	UFUNCTION(BlueprintCallable, meta = (FriendlyName="Add JSON Data"), Category = "JSON")
	void AddData(const FString& key, const FString& value);

	/* Gets JSON data from the post data, by key */
	UFUNCTION(BlueprintCallable, meta = (FriendlyName="Get JSON Data By Key"), Category = "JSON")
	FString GetData(const FString& key);

	/* Creates a new post data object */
	UFUNCTION(BlueprintPure, meta = (FriendlyName="Create JSON Post Data", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "JSON")
	static UPageData* Create(UObject* WorldContextObject);

	/* Posts a request with the supplied post data to the internets */
	UFUNCTION(BlueprintCallable, meta = (FriendlyName="Post JSON Request", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "JSON")
	void PostRequest(UObject* WorldContextObject, const FString& url);

	/* Requests a page from the internet with a JSON response */
	UFUNCTION(BlueprintPure, meta = (FriendlyName="Get JSON Request", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "JSON")
	static UPageData* GetRequest(UObject* WorldContextObject, const FString& url);
};