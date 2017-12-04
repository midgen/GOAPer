// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GOAPState.h"
#include "GOAPEdge.h"
#include "GOAPNode.generated.h"

/**
 * 
 */
USTRUCT()
struct  FGOAPNode
{
	GENERATED_USTRUCT_BODY()
public:
	FGOAPNode() {};
	~FGOAPNode() { };

	FGOAPState State;
	FGOAPEdge Parent;

};

