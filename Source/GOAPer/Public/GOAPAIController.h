// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "GOAPerSettings.h"
#include "GOAPAction.h"
#include "GOAPState.h"
#include "GOAPStateUI.h"
#include "GOAPPlanner.h"
#include "GOAPStateProperty.h"
#include "GOAPPropertyId.h"
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

	UPROPERTY()
	UGOAPerSettings* mySettings = GetMutableDefault<UGOAPerSettings>();

	// The current active Plan
	TQueue<TWeakObjectPtr<UGOAPAction>>	myActionQueue;
	// Current active action
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<UGOAPAction>			CurrentAction;
	// Current goal
	FGOAPStateProperty myCurrentGoal;
	// Current state
	FGOAPState myGOAPState;
	// Available actions for this agent
	UPROPERTY()
	TArray<UGOAPAction*>	myGOAPActions;
	// Queue for managing EQS requests from actions
	
	TQueue<FGOAPEQSJob, EQueueMode::Mpsc>		myEQSJobs;
	FGOAPEQSJob myEQSCurrentJob;
	bool myHasMadeEQSRequest;
	FEnvQueryRequest myEQSRequest;

	/** GOAP actions that will be available to this agent **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	TArray<TSubclassOf<UGOAPAction>> AvailableActions;
	/** Default starting state of the agent **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	FGOAPStateUI StartingState;
	/** Default starting goal of the agent **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	FGOAPStateProperty DefaultGoal;
	/** Maximum number of nodes in the planning graph, prevents crash if graph gets stuck in a loop */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	int32 MaxGraphNodes = 256;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAPer")
	bool DebugDrawCurrentState = false;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Helpers for changing to MoveToStates
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetMoveToStateWithTarget(AActor* TargetActor, const float AcceptanceRadius, const float WalkSpeed);
	// Move to a location, sets movetotarget to current character
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetMoveToStateWithLocation(const FVector Location, const float WalkSpeed);
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
	bool isStateSatisfied(const FGOAPStateProperty aState);
	// Helper to check if a given state is satisfied by the current agent state
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool isStateSatisfied(FGOAPPropertyId Key, const bool Value);

	// Callback for MoveTo completion
	virtual void OnMoveCompleted(FAIRequestID aRequestID, const FPathFollowingResult& aResult) override;

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	FString GetCurrentActionString();
	/** Sets the state of a single atom, creates the entry if it doesn't already exist **/
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetGOAPState(FGOAPPropertyId Key, bool Value);
	/** Gets the state of a single atom **/
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool GetGOAPState(FGOAPPropertyId Key);
	/** Set the current active goal **/
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void SetGOAPGoal(FGOAPPropertyId Key, bool Value);
	UFUNCTION(BlueprintCallable, Category = "GOAP")
	bool IsGoalSet(FGOAPPropertyId Key, bool Value);

	UFUNCTION(BlueprintCallable, Category = "GOAP")
	void AddEQSJob(UGOAPAction* CallingAction, UEnvQuery* Query, TEnumAsByte<EEnvQueryRunMode::Type> RunMode);
	void EQSQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	// Weak pointer to MoveToTarget, used when moving to an actor to abort if the target is destroyed
	TWeakObjectPtr<AActor> myMoveToTargetActor;

};