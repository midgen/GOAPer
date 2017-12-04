#include "GOAPer.h"
#include "GOAPAIController.h"
#include "GameFramework/Actor.h"
#include "GOAPAction.h"
#include "GOAPAtom.h"

UGOAPAction::UGOAPAction(class FObjectInitializer const & ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UGOAPAction::ArePreconditionsSatisfied(AGOAPAIController* controller)
{
	if (PreConditions_Internal.State.Num() == 0)
	{
		return true;
	}

	return controller->GOAPState.IsSatisfiesState(PreConditions_Internal);
}

// Checks if the Agent is within the Interaction Range of the TargetActor
bool UGOAPAction::IsInRange(AGOAPAIController* controller)
{
	// Otherwise, check the distance against the interaction range
	return ((ActionTarget !=  nullptr) && (controller->GetPawn()->GetActorLocation() - ActionTarget->GetActorLocation()).Size() < InteractionRange);
}

bool UGOAPAction::AreEffectsSatisfied(AGOAPAIController* controller)
{
	 return controller->GOAPState.IsSatisfiesState(Effects_Internal);
}


void UGOAPAction::SetupDefaults()
{
	for (FGOAPAtom& pre : PreConditions.State)
	{
		PreConditions_Internal.SetState(pre.Key, pre.Value);
	}

	for (FGOAPAtom& pre : Effects.State)
	{
		Effects_Internal.SetState(pre.Key, pre.Value);
	}

	// Add some randomness to the tick rate to avoid evaluating on the same tick
	TimeSinceLastTick = FMath::FRandRange(0.0f, 1.0f);
	TimeSinceLastCostUpdate = FMath::FRandRange(0.0f, 1.0f);
}
