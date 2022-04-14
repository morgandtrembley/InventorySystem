#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ItemDisplay.generated.h"

UCLASS()
class INVENTORYSYSTEM_API UItemDisplay : public UUserWidget {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableText* ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Thumbnail;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* UseItem;

protected:
	virtual void NativeConstruct() override;
};
