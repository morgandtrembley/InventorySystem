#include "HealthPotionActor.h"
#include "HealthPotion.h"

AHealthPotionActor::AHealthPotionActor() {

	// material set up
	static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("/Game/1MyContent/Materials/RED.RED"));
	Mesh->SetMaterial(0, MaterialAsset.Object);
	
	Tags.Add(TEXT("Pickupable"));
}

void AHealthPotionActor::BeginPlay() {
	Super::BeginPlay();

	Item = NewObject<UHealthPotion>();
}