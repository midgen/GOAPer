#include "GOAPAIController.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "GOAPPlanner.h"


bool AGOAPAIController::LoadGOAPDefaults()
{
	myCurrentGoal = DefaultGoal;

	// Create the actions
	for (TSubclassOf<UGOAPAction> elem : AvailableActions)
	{
		myGOAPActions.Add(NewObject<UGOAPAction>(this, elem));
	}

	for (auto& action : myGOAPActions)
	{
		action->SetupDefaults();
	}

	// Load default state
	for (FGOAPAtom& state : StartingState.State)
	{
		myGOAPState.SetState(state.Key, state.Value);
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
		if (myActionQueue.IsEmpty())
		{
			// If we can form a plan
			if (BuildActionPlanForCurrentGoal())
			{
				// If the goal is already satisfied, trash the queue, we'll try another one next tick
				if (isStateSatisfied(myCurrentGoal))
				{
					myActionQueue.Empty();
					return;
				}
				// Select the head action
				myActionQueue.Dequeue(CurrentAction);
			}
		}
		else // otherwise pop the next action off the queue
		{
			myActionQueue.Dequeue(CurrentAction);
		}
	}
	if (CurrentAction->IsValidLowLevel()) // Should have an action by now, check anyway
	{
		// If preconditions for current action aren't met, the plan is invalid, clear it
		if (!CurrentAction->ArePreconditionsSatisfied(this))
		{
			CurrentAction = nullptr;
			myActionQueue.Empty();
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
	for (auto& action : myGOAPActions)
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
	if (!myEQSCurrentJob.CallingAction.IsValid())
	{
		// See if we have any waiting
		if (myEQSJobs.Dequeue(myEQSCurrentJob))
		{
			myEQSRequest = FEnvQueryRequest(myEQSCurrentJob.Query, GetCharacter());
			myEQSRequest.Execute(myEQSCurrentJob.RunMode, this, &AGOAPAIController::EQSQueryFinished);
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
	myActionQueue.Empty();
}

void AGOAPAIController::SetMoveToStateWithTarget(AActor* TargetActor, const float AcceptanceRadius, const float WalkSpeed)
{
	if (!TargetActor)
	{
		return;
	}

	DrawDebugLine(
		GetWorld(),
		GetPawn()->GetActorLocation(),
		TargetActor->GetActorLocation(),
		FColor(0, 0, 255),
		true, 1.0f, 0,
		12.333
		);

	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	MoveToActor(TargetActor, AcceptanceRadius);
	myMoveToTargetActor = TargetActor;
	_IsMoveCompleted = false;
}

void AGOAPAIController::SetMoveToStateWithLocation(const FVector Location, const float WalkSpeed)
{
	DrawDebugLine(
		GetWorld(),
		GetPawn()->GetActorLocation(),
		Location,
		FColor(0, 255, 0),
		true, 1.0f, 0,
		12.333
		);
	// Set to self to avoid failing null checks
	myMoveToTargetActor = GetCharacter();
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;	
	MoveToLocation(Location, -1.0f);
	_IsMoveCompleted = false;
}

bool AGOAPAIController::BuildActionPlanForCurrentGoal()
{
	TArray<TWeakObjectPtr<UGOAPAction>> planActions;

	// TODO: We only support single state goals right now, awaiting planner upgrade
	FGOAPAtom targetState = myCurrentGoal;

	// The goal is already satisfied, discard it
	if (isStateSatisfied(targetState))
	{
		return false;
	}

	// Make a plan!
	planActions = Planner->Plan(this, MaxGraphNodes, targetState.Key, targetState.Value, myGOAPActions, myGOAPState, *this);

	// If we find one, push it into the Action Queue
	if (planActions.Num() > 0)
	{
		for (int i = 0; i < planActions.Num(); ++i)
		{
			myActionQueue.Enqueue(planActions[i]);
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
	myGOAPState.SetState(Key.Key, Value);
}

bool AGOAPAIController::GetGOAPState(FGOAPAtomKey Key)
{
	if (myGOAPState.State.Contains(Key.Key))
	{
		return *myGOAPState.State.Find(Key.Key);
	}
	else {
		// This is not ideal, but will do for now
		return false;
	}
}

void AGOAPAIController::SetGOAPGoal(FGOAPAtomKey Key, bool Value)
{
	myCurrentGoal.Key = Key.Key;
	myCurrentGoal.Value = Value;
	ClearCurrentActionAndPlan();
}

bool AGOAPAIController::IsGoalSet(FGOAPAtomKey Key, bool Value)
{
	if (myCurrentGoal.Key == Key.Key && myCurrentGoal.Value == Value)
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
	myEQSJobs.Enqueue(job);
}

void AGOAPAIController::EQSQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (myEQSCurrentJob.CallingAction->IsValidLowLevel())
	{
		myEQSCurrentJob.CallingAction->QueryResultsActor.Empty();
		myEQSCurrentJob.CallingAction->QueryResultsLocation.Empty();
		if (myEQSCurrentJob.RunMode == EEnvQueryRunMode::SingleResult)
		{
			myEQSCurrentJob.CallingAction->QueryResultsActor.Add(Result->GetItemAsActor(0));
			myEQSCurrentJob.CallingAction->QueryResultsLocation.Add(Result->GetItemAsLocation(0));
		}
		else
		{
			Result->GetAllAsActors(myEQSCurrentJob.CallingAction->QueryResultsActor);
			Result->GetAllAsLocations(myEQSCurrentJob.CallingAction->QueryResultsLocation);
		}

		myEQSCurrentJob.CallingAction->IsEQSResultsAvailable = true;
		myEQSCurrentJob.CallingAction = nullptr;
		myEQSRequest = nullptr;
	}

}

TArray<UGOAPAction*> AGOAPAIController::GetValidActionsForState(const FGOAPState State)
{
	TArray<UGOAPAction*> result;

	for (auto& action : myGOAPActions)
	{
		if (action->PreConditions_Internal.IsSatisfiesState(State))
		{
			result.Push(action);
		}
	}

	return result;
}

bool AGOAPAIController::isStateSatisfied(FGOAPAtom Atom)
{
	return myGOAPState.IsStateSatisfied(Atom.Key, Atom.Value);
}

bool AGOAPAIController::isStateSatisfied(FGOAPAtomKey Key, bool Value)
{
	return myGOAPState.IsStateSatisfied(Key.Key, Value);
}