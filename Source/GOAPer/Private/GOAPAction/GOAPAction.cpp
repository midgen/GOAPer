#include "GOAPAction.h"
#include "GOAPAIController.h"
#include "GameFramework/Actor.h"

#include "GOAPStateProperty.h"

UGOAPAction::UGOAPAction(class FObjectInitializer const & ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UGOAPAction::ArePreconditionsSatisfied(AGOAPAIController* Controller) const
{
	if (PreConditions_Internal.State.Num() == 0)
	{
		return true;
	}

	return Controller->myGOAPState.SatisfiesState(PreConditions_Internal);
}

// Checks if the Agent is within the Interaction Range of the TargetActor
bool UGOAPAction::IsInRange(AGOAPAIController* Controller)
{
	// Otherwise, check the distance against the interaction range
	return ((ActionTarget != nullptr) && (Controller->GetPawn()->GetActorLocation() - ActionTarget->GetActorLocation()).SizeSquared() < InteractionRange * InteractionRange);
}

bool UGOAPAction::AreEffectsSatisfied(AGOAPAIController* Controller) const
{
	 return Controller->myGOAPState.SatisfiesState(Effects_Internal);
}


void UGOAPAction::SetupDefaults()
{
	for (FGOAPStateProperty& pre : PreConditions.State)
	{
		PreConditions_Internal.SetState(pre.Key, pre.Value);
	}

	for (FGOAPStateProperty& pre : Effects.State)
	{
		Effects_Internal.SetState(pre.Key, pre.Value);
	}

	// Add some randomness to the tick rate to avoid evaluating on the same tick
	TimeSinceLastTick = FMath::FRandRange(0.0f, 1.0f);
	TimeSinceLastCostUpdate = FMath::FRandRange(0.0f, 1.0f);
}
