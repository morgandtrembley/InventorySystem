#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/WrapBox.h"
#include "InventoryDisplay.generated.h"

UCLASS()
class INVENTORYSYSTEM_API UInventoryDisplay : public UUserWidget {
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshInventoryDisplay();

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* Health;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* Shield;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* Weight;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWrapBox* InventoryWrapBox;

protected:
	virtual void NativeConstruct() override;
};
