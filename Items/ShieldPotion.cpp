#include "ShieldPotion.h"
#include "InventorySystemCharacter.h"
#include "PlayerWarning.h"

UShieldPotion::UShieldPotion() {
	Name = FText::FromString("Shield Potion");
	Description = FText::FromString("Gives 50 Shield");
	Weight = 0.5f;
	
	// get and set thumbnail
	static ConstructorHelpers::FObjectFinder<UTexture2D> ThumbnailAsset(TEXT("/Game/1MyContent/Thumbnails/ShieldPotion.ShieldPotion"));
	Thumbnail = ThumbnailAsset.Object;

	StackLimit = 5;
}

bool UShieldPotion::Use(AInventorySystemCharacter* Character) {
	// check for valid character
	if (Character) {
		// handle normal use, overflow if it occurs, and already full shield
		if (Character->Shield < SHIELD_MAX) {
			Character->Shield += POT_VALUE;
			if (Character->Shield > SHIELD_MAX) {
				Character->Shield = SHIELD_MAX;
			}
			return true;
		}
		// shield full, warn player and return false
		else {
			// init warning message and display it - terminates automatically from blueprint
			Character->wPlayerWarning->Warning->SetText(FText::FromString("Shield Full"));
			if (Character->wPlayerWarning->IsInViewport())
				Character->wPlayerWarning->RemoveFromViewport();
			Character->wPlayerWarning->AddToViewport();
		}
	}
	return false;
}
