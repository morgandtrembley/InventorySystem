#pragma once
#include "CoreMinimal.h"
#include "ItemActor.h"
#include "ShieldPotionActor.generated.h"

UCLASS()
class INVENTORYSYSTEM_API AShieldPotionActor : public AItemActor {
	GENERATED_BODY()

	AShieldPotionActor();

	virtual void BeginPlay() override;
};
