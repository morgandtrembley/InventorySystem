#pragma once
#include "CoreMinimal.h"
#include "ItemActor.h"
#include "HealthPotionActor.generated.h"

UCLASS()
class INVENTORYSYSTEM_API AHealthPotionActor : public AItemActor {
	GENERATED_BODY()

	AHealthPotionActor();

	virtual void BeginPlay() override;
};
