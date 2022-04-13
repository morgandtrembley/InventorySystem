#include "GoldActor.h"
#include "Gold.h"

AGoldActor::AGoldActor() {

	// material set up
	static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("/Game/1MyContent/Materials/YELLOW.YELLOW"));
	Mesh->SetMaterial(0, MaterialAsset.Object);
	 
	Tags.Add(TEXT("AutoPickup"));
}

void AGoldActor::BeginPlay() {
	Super::BeginPlay();

	Item = NewObject<UGold>();
}