#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UInventoryComponent();

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<UItem*> DefaultItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UItem*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<int> StackSize;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int Capacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int Gold;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	bool AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);

protected:
	virtual void BeginPlay() override;
};
