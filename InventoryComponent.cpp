#include "InventoryComponent.h"
#include "Item.h"

UInventoryComponent::UInventoryComponent() {
	
}

void UInventoryComponent::BeginPlay() {
	Super::BeginPlay();
	
}

bool UInventoryComponent::AddItem(UItem* Item) {

	return false;
}

bool UInventoryComponent::RemoveItem(UItem* Item) {

	return false;
}