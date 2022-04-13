#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "Gold.generated.h"

UCLASS()
class INVENTORYSYSTEM_API UGold : public UItem {
	GENERATED_BODY()

	UGold();

	virtual bool Use(AInventorySystemCharacter* Character) override;
};