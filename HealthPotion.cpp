#include "HealthPotion.h"
#include "InventorySystemCharacter.h"
#include "PlayerWarning.h"

UHealthPotion::UHealthPotion() {
	Name = FText::FromString("Health Potion");
	Description = FText::FromString("Heals 20 HP");
	Weight = 0.5f;
	
	// Get and set thumbnail
	static ConstructorHelpers::FObjectFinder<UTexture2D> ThumbnailAsset(TEXT("/Game/1MyContent/Thumbnails/HealthPotion.HealthPotion"));
	Thumbnail = ThumbnailAsset.Object;

}

bool UHealthPotion::Use(AInventorySystemCharacter* Character) {
	// check for valid character
	if (Character) {
		// handle normal use, overflow if it occurs, and already full health
		if (Character->Health < HEALTH_MAX) {
			Character->Health += POT_VALUE;
			if (Character->Health > HEALTH_MAX) {
				Character->Health = HEALTH_MAX;
			}
			return true;
		}
		// health full, warn player and return false
		else {
			// init warning message and display it - terminates automatically from blueprint
			Character->wPlayerWarning->Warning->SetText(FText::FromString("Health Full"));
			if (Character->wPlayerWarning->IsInViewport())
				Character->wPlayerWarning->RemoveFromViewport();
			Character->wPlayerWarning->AddToViewport();
		}
	}
	return false;
}