// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GOAPer.h"
#include "AIController.h"
#include "GOAPerSettings.h"
#include "GOAPAction.h"
#include "GOAPState.h"
#include "GOAPStateUI.h"
#include "GOAPPlanner.h"
#include "GOAPAtomKey.h"
#include "GOAPEQSJob.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/Actor.h"
#include "GOAPAIController.generated.h"

UCLASS()
class GOAPER_API AGOAPAIController : public AAIController
{
	GENERATED_BODY()
private:
	bool LoadGOAPDefaults();
	UPROPERTY()
	UGOAPPlanner* Planner;
	bool _IsMoveCompleted = true;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGOAPerSettings* Settings = GetMutableDefault<UGOAPerSettings>();

	// The current active Plan
	TQueue<TWeakObjectPtr<UGOAPAction>>	ActionQueue;
	// Current active action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAPer")
	TWeakObjectPtr<UGOAPAction>			CurrentAction;
	// Current goal
	FGOAPAtom CurrentGoal;
	// Current state
	FGOAPState GOAPState;
	// Available actions for this agent
	UPROPERTY()
	TArray<UGOAPAction*>	GOAPActions;
	// Queue for managing EQS requests from actions
	
	TQueue<FGOAPEQSJob, EQueueMode::Mpsc>		EQSJobs;
	FGOAPEQSJob EQSCurrentJob;
	bool HasMadeEQSRequest;
	FEnvQueryRequest EQSRequest;

	/** GOAP actions that will be available to this agent **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	TArray<TSubclassOf<UGOAPAction>> AvailableActions;
	/** Default starting state of the agent **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	FGOAPStateUI StartingState;
	/** Default starting goal of the agent **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	FGOAPAtom DefaultGoal;
	/** Maximum number of nodes in the planning graph, prevents crash if graph gets stuck in a loop */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	int32 MaxGraphNodes = 256;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Helpers for changing to MoveToStates
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetMoveToStateWithTarget(AActor* aTargetActor, const float aAcceptanceRadius, const float WalkSpeed);
	// Move to a location, sets movetotarget to current character
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetMoveToStateWithLocation(const FVector aLocation, const float WalkSpeed);
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool IsMoveCompleted() { return _IsMoveCompleted; }

	/*** Planner-related function ***/
	// DWISOTT - Commonly used when perception stimulus causes us to need to replan
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void ClearCurrentActionAndPlan();
	// Activate the Planner for the current goal
	bool BuildActionPlanForCurrentGoal();
	// Helper function for the planner, fetch the valid actions for a given state
	TArray<UGOAPAction*> GetValidActionsForState(const FGOAPState aState);
	// Helper to check if a given state is satisfied by the current agent state
	bool isStateSatisfied(const FGOAPAtom aState);
	// Helper to check if a given state is satisfied by the current agent state
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool isStateSatisfied(FGOAPAtomKey Key, const bool Value);

	// Callback for MoveTo completion
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	FString GetCurrentActionString();
	/** Sets the state of a single atom, creates the entry if it doesn't already exist **/
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetGOAPState(FGOAPAtomKey Key, bool Value);
	/** Gets the state of a single atom **/
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool GetGOAPState(FGOAPAtomKey Key);
	/** Set the current active goal **/
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetGOAPGoal(FGOAPAtomKey Key, bool Value);
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool IsGoalSet(FGOAPAtomKey Key, bool Value);

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void AddEQSJob(UGOAPAction* CallingAction, UEnvQuery* Query, TEnumAsByte<EEnvQueryRunMode::Type> RunMode);
	void EQSQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// Weak pointer to MoveToTarget, used when moving to an actor to abort if the target is destroyed
	TWeakObjectPtr<AActor> MoveToTargetActor;

};