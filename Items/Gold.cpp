#include "Gold.h"
#include "InventorySystemCharacter.h"
#include "InventoryComponent.h"

UGold::UGold() {
	Name = FText::FromString("Gold");
	Description = FText::FromString("Currency");
	Weight = 0.0f;
}

// adds gold to inventory
bool UGold::Use(AInventorySystemCharacter* Character) {
	if (Character) {
		Character->Inventory->Gold += 1;
		Character->Inventory->Weight += Weight;
		return true;
	}
	return false;
}
