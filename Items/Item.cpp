#include "Item.h"
#include "InventoryComponent.h"
#include "InventorySystemCharacter.h"
#include "PlayerWarning.h"

UItem::UItem() {
	Name = FText::FromString("Item Name");
	Description = FText::FromString("Item\nDescription");
	StackLimit = 10;
	Weight = 0.1f;
}

bool UItem::Use(AInventorySystemCharacter* Character) {
	return false;
}
