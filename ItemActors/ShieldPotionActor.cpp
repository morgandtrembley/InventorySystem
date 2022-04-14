#include "ShieldPotionActor.h"
#include "ShieldPotion.h"

AShieldPotionActor::AShieldPotionActor() {

	// material set up
	static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("/Game/1MyContent/Materials/BLUE.BLUE"));
	Mesh->SetMaterial(0, MaterialAsset.Object);

	Tags.Add(TEXT("Pickupable"));
}

void AShieldPotionActor::BeginPlay() {
	Super::BeginPlay();

	Item = NewObject<UShieldPotion>();
}
