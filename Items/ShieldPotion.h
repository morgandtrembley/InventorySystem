#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ShieldPotion.generated.h"

#define POT_VALUE 50.0f

UCLASS()
class INVENTORYSYSTEM_API UShieldPotion : public UItem {
	GENERATED_BODY()
	
	UShieldPotion();

	virtual bool Use(AInventorySystemCharacter* Character) override;
};
