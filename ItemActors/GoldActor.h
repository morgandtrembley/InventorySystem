#pragma once
#include "CoreMinimal.h"
#include "ItemActor.h"
#include "GoldActor.generated.h"

class UItem;

UCLASS()
class INVENTORYSYSTEM_API AGoldActor : public AItemActor {
	GENERATED_BODY()

	AGoldActor();
	
protected:
	virtual void BeginPlay() override;
};
