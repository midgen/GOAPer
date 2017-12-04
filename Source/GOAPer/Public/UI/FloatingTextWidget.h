// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "GOAPAIController.h"
#include "FloatingTextWidget.generated.h"


/**
 * 
 */
UCLASS()
class GOAPER_API UFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display Text")
	FString DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display Text")
	FSlateColor TextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Ref")
	AGOAPAIController* ControllerRef;
	

};
