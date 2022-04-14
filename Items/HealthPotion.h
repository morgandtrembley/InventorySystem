#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "HealthPotion.generated.h"

#define POT_VALUE 20.0f

UCLASS()
class INVENTORYSYSTEM_API UHealthPotion : public UItem {
	GENERATED_BODY()

	UHealthPotion();
	
	virtual bool Use(AInventorySystemCharacter* Character) override;
};
