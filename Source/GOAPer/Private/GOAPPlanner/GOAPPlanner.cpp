#include "GOAPPlanner.h"
#include "GOAPNode.h"
#include "GOAPStateProperty.h"
#include "GOAPAction.h"
#include "GOAPAIController.h"



UGOAPPlanner::UGOAPPlanner(const FObjectInitializer &ObjectInitializer) :Super(ObjectInitializer)
{

}
/**
/*  Form a plan to satisfy the specified target state
/   This needs a fairly comprehensive rework soon!
/   Uses a very basic forward search from current state at present.
**/
bool UGOAPPlanner::Plan(UObject* aOuter, const int32 aMaxNodes, const uint8 aPropertyId, const bool aValue, const TArray<UGOAPAction*>& aActions, FGOAPState& aCurrentState, AGOAPAIController& controller, TArray<TWeakObjectPtr<UGOAPAction>>& aOutPlan)
{
	
	OpenNodes.Empty();
	ClosedNodes.Empty(0);	
	ClosedNodes.Reserve(aMaxNodes);

	// First build the graph, start building from current state
	FGOAPNode startNode;
	startNode.State = aCurrentState;

	OpenNodes.Push(startNode);

	while (OpenNodes.Num() > 0)
	{
		ClosedNodes.Push(OpenNodes.Pop(false));

		int32 workNodeIndex = ClosedNodes.Num() - 1;

		FGOAPNode& workNode = ClosedNodes[workNodeIndex];

		// Don't continue searching this branch if state is already satisfied
		if (!workNode.State.SatisfiesProperty(aPropertyId, aValue))
		{
			// Get all valid actions for this state to check
			for (auto& action : controller.GetValidActionsForState(workNode.State))
			{
				// Make sure we aren't caught in an action loop
				// e.g. EnterCover->ExitCover->EnterCover->ExitCover
				if (!workNode.Parent.Action.IsValid() || !ClosedNodes[workNode.Parent.NodeIndex].Parent.Action.IsValid() || action != ClosedNodes[ClosedNodes[workNode.Parent.NodeIndex].Parent.NodeIndex].Parent.Action)
				{
					//FGOAPNode& grandParentNode = ClosedNodes[ClosedNodes[workNode.Parent.NodeIndex].Parent.NodeIndex].Parent.Action;
					//if (action == grandParentNode.Parent.Action) {
					//	continue;
					//}
					// The new node is the previous state plus the effects of the action that got us here
					FGOAPNode newNode;
					newNode.State = workNode.State;
					newNode.State += action->Effects_Internal;
					newNode.Parent.Action = action;
					newNode.Parent.NodeIndex = workNodeIndex;
					OpenNodes.Push(newNode);
				}
			}
		}

		if (ClosedNodes.Num() >= aMaxNodes)
		{
			// We've run out of nodes, usually because of a circular route
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Planning fail, circular routes?"));
			return false;

		}

	}

	TArray<const FGOAPNode*> ValidNodes;

	// Now find any nodes that meet the target state
	for (const FGOAPNode& node : ClosedNodes)
	{
		if (node.State.SatisfiesProperty(aPropertyId, aValue))
		{
			ValidNodes.Push(&node);
		}
	}

	// Now compile the list of plans
	TArray<TArray<TWeakObjectPtr<UGOAPAction>>> ValidPlans;

	for (const FGOAPNode* node : ValidNodes)
	{
		const FGOAPNode* CurrentNode = node;
		TArray<TWeakObjectPtr<UGOAPAction>>& CandidatePlan = ValidPlans[ValidPlans.Emplace()];
		while(CurrentNode->Parent.Action.IsValid())
		{
			CandidatePlan.Push(CurrentNode->Parent.Action);
			CurrentNode = &ClosedNodes[CurrentNode->Parent.NodeIndex];
		}
	}

	// Now pick the plan with the lowest cost
	int32 shortestPlan = INT_MAX;
	int16 index = 0;
	int16 shortPlanIndex = -1;
	for (auto& plan : ValidPlans)
	{
		int32 thisPlanCost = 0;
		for (auto& planAction : plan) 
		{
			thisPlanCost += planAction->Cost;
		}

		if (thisPlanCost < shortestPlan)
		{
			shortestPlan = thisPlanCost;
			shortPlanIndex = index;
		}
		++index;
	}
	if (shortPlanIndex > -1)
	{
		// Need to reverse the plan
		while (ValidPlans[shortPlanIndex].Num() > 0)
		{
			aOutPlan.Push(ValidPlans[shortPlanIndex].Pop());
		}
	}

	if (aOutPlan.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Planning failed, no plan found: " + FString::FromInt(ClosedNodes.Num())));
		return false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Planning complete, nodes: " + FString::FromInt(ClosedNodes.Num()) +
			", valid plans: " + FString::FromInt(ValidPlans.Num()) +
			", optimal plan Cost: " + FString::FromInt(shortestPlan)));

	}

	
	FString planString;
	for (auto& action : aOutPlan)
	{
		planString += action->ActionDescription + TEXT(":");
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, planString);

	return true;
}
