#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UBoxComponent;
class UItem;

UCLASS()
class INVENTORYSYSTEM_API AItemActor : public AActor {
	GENERATED_BODY()
	
public:	
	AItemActor();

	UPROPERTY(VisibleAnywhere)
	UItem* Item;

	UPROPERTY()
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	UBoxComponent* Collider;


protected:
	virtual void BeginPlay() override;

};
