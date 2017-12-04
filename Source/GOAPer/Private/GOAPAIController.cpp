// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAPer.h"
#include "GOAPAIController.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "GOAPPlanner.h"


bool AGOAPAIController::LoadGOAPDefaults()
{
	CurrentGoal = DefaultGoal;

	// Create the actions
	for (TSubclassOf<UGOAPAction> elem : AvailableActions)
	{
		GOAPActions.Add(NewObject<UGOAPAction>(this, elem));
	}

	for (auto& action : GOAPActions)
	{
		action->SetupDefaults();
	}

	// Load default state
	for (FGOAPAtom& state : StartingState.State)
	{
		GOAPState.SetState(state.Key, state.Value);
	}

	return true;
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();

	Planner = NewObject<UGOAPPlanner>(this);

	LoadGOAPDefaults();

	//SetNewState(MakeShareable(new IdleState()));
}

void AGOAPAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we don't have a current action waiting....
	if (CurrentAction == nullptr)
	{
		// If there's nothing queued, we need to try and form a plan
		if (ActionQueue.IsEmpty())
		{
			// If we can form a plan
			if (BuildActionPlanForCurrentGoal())
			{
				// If the goal is already satisfied, trash the queue, we'll try another one next tick
				if (isStateSatisfied(CurrentGoal))
				{
					ActionQueue.Empty();
					return;
				}
				// Select the head action
				ActionQueue.Dequeue(CurrentAction);
			}
		}
		else // otherwise pop the next action off the queue
		{
			ActionQueue.Dequeue(CurrentAction);
		}
	}
	if (CurrentAction->IsValidLowLevel()) // Should have an action by now, check anyway
	{
		// If preconditions for current action aren't met, the plan is invalid, clear it
		if (!CurrentAction->ArePreconditionsSatisfied(this))
		{
			CurrentAction = nullptr;
			ActionQueue.Empty();
		}
		// If effects are satisfied, just complete the action without clearing plan
		else if (CurrentAction->AreEffectsSatisfied(this))
		{
			CurrentAction = nullptr;
		}
		else
		{
			// Otherwise, crack on with it
			CurrentAction->TimeSinceLastTick += DeltaTime;
			if (CurrentAction->TimeSinceLastTick > CurrentAction->TickRate)
			{
				CurrentAction->TimeSinceLastTick = 0.0f;
				// Run the action, if it returns true, it's complete,
				if (CurrentAction->Execute(this, DeltaTime))
				{
					// And clear the action so the next will be popped from the queue on next tick
					CurrentAction = nullptr;
				}
			}
		}
	}

	// Check for action cost update ticks
	for (auto& action : GOAPActions)
	{
		action->TimeSinceLastCostUpdate += DeltaTime;
		if (action->TimeSinceLastCostUpdate > action->CostUpdateRate)
		{
			action->TimeSinceLastCostUpdate = 0;
			action->UpdateCost(this);
		}
	}

	// Process EQS Jobs
	// If we don't have an active job
	if (!EQSCurrentJob.CallingAction.IsValid())
	{
		// See if we have any waiting
		if (EQSJobs.Dequeue(EQSCurrentJob))
		{
			EQSRequest = FEnvQueryRequest(EQSCurrentJob.Query, GetCharacter());
			EQSRequest.Execute(EQSCurrentJob.RunMode, this, &AGOAPAIController::EQSQueryFinished);
		}
	}
}

void AGOAPAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	_IsMoveCompleted = true;
}



void AGOAPAIController::ClearCurrentActionAndPlan()
{
	// clearing the action and queue will cause IdleState to form a new plan
	CurrentAction = nullptr;
	ActionQueue.Empty();
}

void AGOAPAIController::SetMoveToStateWithTarget(AActor* aTargetActor, const float aAcceptanceRadius, const float WalkSpeed)
{
	if (!aTargetActor)
	{
		return;
	}

	DrawDebugLine(
		GetWorld(),
		GetPawn()->GetActorLocation(),
		aTargetActor->GetActorLocation(),
		FColor(0, 0, 255),
		true, 1.0f, 0,
		12.333
		);

	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	MoveToActor(aTargetActor, aAcceptanceRadius);
	MoveToTargetActor = aTargetActor;
	_IsMoveCompleted = false;
}

void AGOAPAIController::SetMoveToStateWithLocation(const FVector aLocation, const float WalkSpeed)
{
	DrawDebugLine(
		GetWorld(),
		GetPawn()->GetActorLocation(),
		aLocation,
		FColor(0, 255, 0),
		true, 1.0f, 0,
		12.333
		);
	// Set to self to avoid failing null checks
	MoveToTargetActor = GetCharacter();
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;	
	MoveToLocation(aLocation, -1.0f);
	_IsMoveCompleted = false;
}

bool AGOAPAIController::BuildActionPlanForCurrentGoal()
{
	TArray<TWeakObjectPtr<UGOAPAction>> planActions;

	// TODO: We only support single state goals right now, awaiting planner upgrade
	FGOAPAtom targetState = CurrentGoal;

	// The goal is already satisfied, discard it
	if (isStateSatisfied(targetState))
	{
		return false;
	}

	// Make a plan!
	planActions = Planner->Plan(this, MaxGraphNodes, targetState.Key, targetState.Value, &GOAPActions, &GOAPState, *this);

	// If we find one, push it into the Action Queue
	if (planActions.Num() > 0)
	{
		for (int i = 0; i < planActions.Num(); ++i)
		{
			ActionQueue.Enqueue(planActions[i]);
		}
	}
	else
	{
		return false;
	}

	return true;
}


FString AGOAPAIController::GetCurrentActionString()
{
	if (CurrentAction != nullptr)
	{
		return CurrentAction->ActionDescription;
	}
	else
	{
		return TEXT("None");
	}
}



void AGOAPAIController::SetGOAPState(FGOAPAtomKey Key, bool Value)
{
	GOAPState.SetState(Key.Key, Value);
}

bool AGOAPAIController::GetGOAPState(FGOAPAtomKey Key)
{
	if (GOAPState.State.Contains(Key.Key))
	{
		return *GOAPState.State.Find(Key.Key);
	}
	else {
		// This is not ideal, but will do for now
		return false;
	}
}

void AGOAPAIController::SetGOAPGoal(FGOAPAtomKey Key, bool Value)
{
	CurrentGoal.Key = Key.Key;
	CurrentGoal.Value = Value;
	ClearCurrentActionAndPlan();
}

bool AGOAPAIController::IsGoalSet(FGOAPAtomKey Key, bool Value)
{
	if (CurrentGoal.Key == Key.Key && CurrentGoal.Value == Value)
	{
		return true;
	}

	return false;
}

void AGOAPAIController::AddEQSJob(UGOAPAction* CallingAction, UEnvQuery* Query, TEnumAsByte<EEnvQueryRunMode::Type> RunMode)
{
	FGOAPEQSJob job;
	job.CallingAction = CallingAction;
	job.Query = Query;
	job.RunMode = RunMode;
	EQSJobs.Enqueue(job);
}

void AGOAPAIController::EQSQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (EQSCurrentJob.CallingAction->IsValidLowLevel())
	{
		EQSCurrentJob.CallingAction->QueryResultsActor.Empty();
		EQSCurrentJob.CallingAction->QueryResultsLocation.Empty();
		if (EQSCurrentJob.RunMode == EEnvQueryRunMode::SingleResult)
		{
			EQSCurrentJob.CallingAction->QueryResultsActor.Add(Result->GetItemAsActor(0));
			EQSCurrentJob.CallingAction->QueryResultsLocation.Add(Result->GetItemAsLocation(0));
		}
		else
		{
			Result->GetAllAsActors(EQSCurrentJob.CallingAction->QueryResultsActor);
			Result->GetAllAsLocations(EQSCurrentJob.CallingAction->QueryResultsLocation);
		}

		EQSCurrentJob.CallingAction->IsEQSResultsAvailable = true;
		EQSCurrentJob.CallingAction = nullptr;
		EQSRequest = nullptr;
	}

}

TArray<UGOAPAction*> AGOAPAIController::GetValidActionsForState(const FGOAPState aState)
{
	TArray<UGOAPAction*> result;

	for (auto& action : GOAPActions)
	{
		if (action->PreConditions_Internal.IsSatisfiesState(aState))
		{
			result.Push(action);
		}
	}

	return result;
}

bool AGOAPAIController::isStateSatisfied(FGOAPAtom aAtom)
{
	return GOAPState.IsStateSatisfied(aAtom.Key, aAtom.Value);
}

bool AGOAPAIController::isStateSatisfied(FGOAPAtomKey Key, bool Value)
{
	return GOAPState.IsStateSatisfied(Key.Key, Value);
}