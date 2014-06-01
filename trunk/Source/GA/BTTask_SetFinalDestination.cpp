#include "GA.h"
#include "GACharacter.h"
#include "GAEnemy.h"
#include "BTTask_SetFinalDestination.h"


UBTTask_SetFinalDestination::UBTTask_SetFinalDestination(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

EBTNodeResult::Type UBTTask_SetFinalDestination::ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory)
{
	for (TActorIterator<ATargetPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		if (ActorItr->GetName() == "BaseLocation") {
			OwnerComp->GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), *ActorItr);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}