#pragma once
#include "GOAPStateProperty.h"
#include "GOAPState.h"
#include "GOAPStateUI.h"
#include "GOAPAction.generated.h"


class AGOAPAIController;

UCLASS(Blueprintable, BlueprintType, meta = (ShowWorldContextPin))
class GOAPER_API UGOAPAction : public UObject
{
	GENERATED_BODY()

public:
	UGOAPAction(const class FObjectInitializer& ObjectInitializer);
	// Checks if the preconditions are satisfied by the current agent state
	UFUNCTION(BlueprintPure, Category = "GOAP Action")
	bool ArePreconditionsSatisfied(AGOAPAIController* Controller) const;

	// Checks if the agent is within range of whatever it needs to be in range of
	UFUNCTION(BlueprintPure, Category = "GOAP Action")
	bool IsInRange(AGOAPAIController* Controller);

	// Checks if the target state (effect) is already satisfied
	UFUNCTION(BlueprintPure, Category = "GOAP Action")
	bool AreEffectsSatisfied(AGOAPAIController* Controller) const;

	// Called each frame while this is the active action, returns true when complete
	//   Can/should be overridden in Blueprint
	UFUNCTION(BlueprintNativeEvent, Category = "GOAP Action")
	bool Execute(AGOAPAIController* Controller, float DeltaSeconds);
	virtual bool Execute_Implementation(AGOAPAIController* Controller, float DeltaSeconds) { return false;};

	// Called each frame while this is the active action, returns true when complete
	//   Can/should be overridden in Blueprint
	UFUNCTION(BlueprintNativeEvent, Category = "GOAP Action")
	void UpdateCost(AGOAPAIController* Controller);
	virtual void UpdateCost_Implementation(AGOAPAIController* Controller) { };

	//Text description of the action for debugging
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAP Action")
	FString ActionDescription;
	// How often Execute should be run, default 0 = every tick
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAP Action")
	float TickRate;
	float TimeSinceLastTick = 0.0f;
	// Cost of the action, used for planning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP Action")
	int32 Cost;
	// Frequency to call the UpdateCost function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP Action")
	float CostUpdateRate = 60.0f;
	float TimeSinceLastCostUpdate;
	// Pointer to the target Actor of this action (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP Action")
	TWeakObjectPtr<AActor> ActionTarget = nullptr;
	// Required range from target to perform action, if outside this range, the agent will move towards it
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAP Action")
	float InteractionRange;
	// If this action is active, should perception stimuli interrupt MoveTo state
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAP Action")
	bool ShouldInterruptMoveOnTargetAcquisition = false;

	// Internal state of the preconditions and effects
	FGOAPState PreConditions_Internal;
	FGOAPState Effects_Internal;
	// UI inputs for State that must be satisfied for this to be a valid action
	//  Values are copied to the _Internal versions on startup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAP Action")
	FGOAPStateUI PreConditions;
	// State that is applied on successful completion of this action
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GOAP Action")
	FGOAPStateUI Effects;

	// Helpers for the use of EQS queries
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "GOAP Action")
	bool IsEQSQueryRequestPending;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "GOAP Action")
	bool IsEQSResultsAvailable;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GOAP Action")
	TArray<AActor*> QueryResultsActor;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GOAP Action")
	TArray<FVector> QueryResultsLocation;

	// Configures the action, copying data from UI structs to Internal
	void SetupDefaults();


};
