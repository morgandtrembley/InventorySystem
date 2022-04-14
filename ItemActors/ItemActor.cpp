#include "ItemActor.h"
#include "Components/BoxComponent.h"
#include "Item.h"

AItemActor::AItemActor() {

	// visual mesh set up
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Game/1MyContent/Meshes/PotionMesh.PotionMesh"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetStaticMesh(MeshAsset.Object);
	Mesh->SetWorldScale3D(FVector(0.2f));
	
	// collider set up for pick up range
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");
	Collider->SetupAttachment(Mesh);
	Collider->AddRelativeLocation(FVector(0.0f,0.0f,50.0f));
	Collider->SetWorldScale3D(FVector(12));

	Tags.Add(TEXT("Item"));
}

// adds Item UObject to actor for easy transfer to inventory
void AItemActor::BeginPlay() {
	Super::BeginPlay();
	
}
