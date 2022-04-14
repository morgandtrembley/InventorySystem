#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "PlayerWarning.generated.h"

UCLASS()
class INVENTORYSYSTEM_API UPlayerWarning : public UUserWidget {
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableText* Warning;

protected:
	virtual void NativeConstruct() override;
};
