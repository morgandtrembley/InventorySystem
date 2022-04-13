// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventorySystemCharacter.generated.h"

#define HEALTH_MAX 100.0f
#define SHIELD_MAX 100.0f
#define WEIGHT_MAX 100.0f

class UInventoryComponent;
class UPickupNotification;
class UInventoryDisplay;
class UPlayerWarning;
class UItemDisplay;
class AItemActor;
class UItem;
class UGold;

UCLASS(config=Game)
class AInventorySystemCharacter : public ACharacter {
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AInventorySystemCharacter();

	APlayerController* MyController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AItemActor* OverlappedActor;

	TSubclassOf<UUserWidget> PickupNotificationClass;
	UPickupNotification* wPickupNotification;

	TSubclassOf<UUserWidget> InventoryDisplayClass;
	UInventoryDisplay* wInventoryDisplay;

	TSubclassOf<UUserWidget> ItemDisplayClass;
	UItemDisplay* wItemDisplay;

	TSubclassOf<UUserWidget> WarningClass;
	UPlayerWarning* wPlayerWarning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInventoryComponent* Inventory;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Shield;

	

protected:
	UFUNCTION(BlueprintCallable)
	void UseItem(UItem* Item);

	void ToggleInventory();

	void PickupItem();

	// Overlap Events
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:

	void UpdateInventoryDisplay();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

private:

	TArray<AItemActor*> OverlappedItemActors;
	AItemActor* ToDestroy;

	FString DisplayMessage;

	bool DisplayMessageflag;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

