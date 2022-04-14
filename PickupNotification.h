#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "PickupNotification.generated.h"

UCLASS()
class INVENTORYSYSTEM_API UPickupNotification : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* DisplayMessage;

protected:
	virtual void NativeConstruct() override;
};