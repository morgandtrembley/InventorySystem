// Copyright Epic Games, Inc. All Rights Reserved.
#include "InventorySystemCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.h"
#include "PickupNotification.h"
#include "InventoryDisplay.h"
#include "PlayerWarning.h"
#include "ItemDisplay.h"
#include "ItemActor.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////
// AInventorySystemCharacter

AInventorySystemCharacter::AInventorySystemCharacter() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//Overlap Events
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AInventorySystemCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AInventorySystemCharacter::OnOverlapEnd);

	// Character data initialization
	Inventory = CreateDefaultSubobject<UInventoryComponent>("Inventory");
	Inventory->Capacity = 30;
	Inventory->Gold = 0;
	Inventory->Weight = 0.0f;
	Health = 50.0f;
	Shield = 0.0f;

	// Blueprint class set up
	static ConstructorHelpers::FClassFinder<UUserWidget> PickupBPClass(TEXT("/Game/1MyContent/Blueprints/PickupNotificationBP"));
	PickupNotificationClass = PickupBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryDisplayBPClass(TEXT("/Game/1MyContent/Blueprints/InventoryDisplayBP"));
	InventoryDisplayClass = InventoryDisplayBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> ItemDisplayBPClass(TEXT("/Game/1MyContent/Blueprints/ItemDisplayBP"));
	ItemDisplayClass = ItemDisplayBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> WarningBPClass(TEXT("/Game/1MyContent/Blueprints/PlayerWarningBP"));
	WarningClass = WarningBPClass.Class;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AInventorySystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AInventorySystemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AInventorySystemCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AInventorySystemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AInventorySystemCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AInventorySystemCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AInventorySystemCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AInventorySystemCharacter::OnResetVR);

	//Inventory Display Toggle
	PlayerInputComponent->BindAction("ToggleInventory", IE_Pressed, this, &AInventorySystemCharacter::ToggleInventory);

	// Handle Picked Up Items
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AInventorySystemCharacter::PickupItem);
}

void AInventorySystemCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	// Item Handling
	if (OtherActor->ActorHasTag("Item")) {

		// Simple Handling for actors that do not need overlap protection (These actors have 0 weight)
		if (OtherActor->ActorHasTag("AutoPickup")) {
			UseItem(Cast<AItemActor>(OtherActor)->Item);
			OtherActor->Destroy();
		}

		// Handling for actors that require input to be picked up and added to inventory
		if (OtherActor->ActorHasTag("Pickupable")) {

			// add to overlapped actors stack
			OverlappedItemActors.Add(Cast<AItemActor>(OtherActor));

			// Handle message display to inform player they can pick up this item
			DisplayMessage = "Press E to pick up ";
			DisplayMessage.Append(Cast<AItemActor>(OtherActor)->Item->Name.ToString());
			wPickupNotification->DisplayMessage->SetText(FText::FromString(DisplayMessage));
		}
	}
}

void AInventorySystemCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	
	// Check for pickupable actors only (they are the only entries to overlapped actor stack)
	if (OtherActor->ActorHasTag("Pickupable")) {

		// if actor is on top of stack, Remove from stack
		if (OverlappedItemActors.IndexOfByKey(Cast<AItemActor>(OtherActor)) == OverlappedItemActors.Num() - 1) {
			OverlappedItemActors.Remove(Cast<AItemActor>(OtherActor));

			// if there are still overlapped actors, update displayed message to top of stack
			if (OverlappedItemActors.Num() > 0) {
				DisplayMessage = "Press E to pick up ";
				DisplayMessage.Append(OverlappedItemActors.Last()->Item->Name.ToString());
				wPickupNotification->DisplayMessage->SetText(FText::FromString(DisplayMessage));
			}

			// if no overlapped actors, clear message
			else {
				wPickupNotification->DisplayMessage->SetText(FText::FromString(""));
			}
		}

		// if actor is not on top of stack, simple removal is sufficient
		else {
			OverlappedItemActors.Remove(Cast<AItemActor>(OtherActor));
		}
	}
}

void AInventorySystemCharacter::UseItem(UItem* Item) {
	int i, index = -1;

	// check for valid item
	if (Item) {

		// use item (returns true if use is successful)
		if (Item->Use(this)) {

			// verify there are elements in array before for loop
			if (Inventory->Items.Num() > 0) {

				// find index of last occurance of item in inventory if it exists
				for (i = 0; i < Inventory->Items.Num(); i++) {
					if (Inventory->Items[i]->GetClass() == Item->GetClass()) {
						index = i;
					}
				}
			}

			// this protects against use of auto-pickup items since they aren't represented in the inventory stack
			if (index != -1) {

				// remove item from inventory
				if (Inventory->StackSize[index] > 1) {
					Inventory->StackSize[index]--;
				}
				else {
					Inventory->Items.RemoveAt(index);
				}

				// update weight and inventory display
				Inventory->Weight -= Item->Weight;
				UpdateInventoryDisplay();
			}
		}
	}
}

void AInventorySystemCharacter::UpdateInventoryDisplay() {

	// update inventory components for display
	DisplayMessage = "Health: " + FString::SanitizeFloat(Health);
	wInventoryDisplay->Health->SetText(FText::FromString(DisplayMessage));

	DisplayMessage = "Shield: " + FString::SanitizeFloat(Shield);
	wInventoryDisplay->Shield->SetText(FText::FromString(DisplayMessage));

	DisplayMessage = "Weight: " + FString::SanitizeFloat(Inventory->Weight);
	wInventoryDisplay->Weight->SetText(FText::FromString(DisplayMessage));

	DisplayMessage = "Gold: " + FString::FromInt(Inventory->Gold);
	wInventoryDisplay->Gold->SetText(FText::FromString(DisplayMessage));

	// refresh inventory display
	wInventoryDisplay->RefreshInventoryDisplay();
}

void AInventorySystemCharacter::ToggleInventory() {

	// calls blueprint function to update inventory information
	UpdateInventoryDisplay();
	
	// toggle functionality
	if (wInventoryDisplay->IsInViewport()) {
		wInventoryDisplay->RemoveFromViewport();

		// toggle cursor capabilities and movement
		if (MyController) {
			MyController->bShowMouseCursor = false;
			MyController->bEnableClickEvents = false;
			MyController->bEnableMouseOverEvents = false;
			MyController->SetIgnoreLookInput(false);
			MyController->SetIgnoreMoveInput(false);
		}

		// handle overlapped item display message
		if (DisplayMessageflag) { 
			wPickupNotification->AddToViewport(); DisplayMessageflag = false; 
		}
	}
	else {

		// handle overlapped item display message
		if (wPickupNotification->IsInViewport()) {
			wPickupNotification->RemoveFromViewport();
			DisplayMessageflag = true;
		}

		// children of inventory wrap box (construction handles adding inventory items to wrap box)
		wInventoryDisplay->InventoryWrapBox->ClearChildren();
		wInventoryDisplay->AddToViewport();

		// toggle cursor capabilities and movement
		if (MyController) {
			MyController->bShowMouseCursor = true;
			MyController->bEnableClickEvents = true;
			MyController->bEnableMouseOverEvents = true;
			MyController->SetIgnoreLookInput(true);
			MyController->SetIgnoreMoveInput(true);
		}
	}
}

// compare strings
bool Compare(FText A, FText B) {
	if (A.ToString().Compare(B.ToString()) == 0) {
		return true;
	}
	return false;
}

void AInventorySystemCharacter::PickupItem() {
	int i, ItemIndex = -1;

	// check for any actors in range to pick up
	if (OverlappedItemActors.Num() > 0) {

		// if weight limit will not be reached on pick up
		if (Inventory->Weight + OverlappedItemActors.Last()->Item->Weight <= WEIGHT_MAX) {

			// Check if Actor class is already represented in Inventory
			if (Inventory->Items.Num() > 0) {
				for (i = 0; i < Inventory->Items.Num(); i++) {
					if (Compare(Inventory->Items[i]->Name, OverlappedItemActors.Last()->Item->Name)) {
						ItemIndex = i;
					}
				}
			}

			// if Actor class is not in Inventory
			if (ItemIndex == -1) {

				// if inventory is not full, add actor class
				if (Inventory->Items.Num() < Inventory->Capacity) {
					Inventory->Items.Add(OverlappedItemActors.Last()->Item);
					Inventory->StackSize.Add(1);
					Inventory->Weight += OverlappedItemActors.Last()->Item->Weight;

					// destory Actor (Must Remove from stack in end overlap function)
					OverlappedItemActors.Last()->Destroy();
					
				}
				else {

					// display inventory is full
					wPlayerWarning->Warning->SetText(FText::FromString("Inventory Full"));
					if (wPlayerWarning->IsInViewport())
						wPlayerWarning->RemoveFromViewport();
					wPlayerWarning->AddToViewport();
				}
			}

			// if actor class is already in Inventory, increment stack count for class
			else {
				if (Inventory->StackSize[ItemIndex] < OverlappedItemActors.Last()->Item->StackLimit) {
					Inventory->StackSize[ItemIndex] += 1;
					Inventory->Weight += OverlappedItemActors.Last()->Item->Weight;

					// destory Actor (Must Remove from overlapped items array in end overlap function)
					OverlappedItemActors.Last()->Destroy();
				}
				else {
					if (Inventory->Items.Num() < Inventory->Capacity) {

						// add to inventory items
						Inventory->Items.Add(OverlappedItemActors.Last()->Item);
						Inventory->StackSize.Add(1);
						Inventory->Weight += OverlappedItemActors.Last()->Item->Weight;

						// destory Actor (Must Remove from overlapped items array in end overlap function)
						OverlappedItemActors.Last()->Destroy();
					}
					else {

						// display inventory is full
						wPlayerWarning->Warning->SetText(FText::FromString("Inventory Full"));
						if (wPlayerWarning->IsInViewport())
							wPlayerWarning->RemoveFromViewport();
						wPlayerWarning->AddToViewport();
					}
				}
				
				// DEBUG: display name of item added to inventory
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Inventory->Items[ItemIndex]->Name.ToString());
			}
		}
		else { 

			// display weight limit reached
			wPlayerWarning->Warning->SetText(FText::FromString("Carrying Too Much Weight"));
			if (wPlayerWarning->IsInViewport())
				wPlayerWarning->RemoveFromViewport();
			wPlayerWarning->AddToViewport();
		}
	}
}

void AInventorySystemCharacter::OnResetVR() {
	// If InventorySystem is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in InventorySystem.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AInventorySystemCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location) {
		Jump();
}

void AInventorySystemCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location) {
		StopJumping();
}

void AInventorySystemCharacter::TurnAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AInventorySystemCharacter::LookUpAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AInventorySystemCharacter::MoveForward(float Value) {
	if ((Controller != nullptr) && (Value != 0.0f)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AInventorySystemCharacter::MoveRight(float Value) {
	if ( (Controller != nullptr) && (Value != 0.0f) ) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AInventorySystemCharacter::BeginPlay() {
	Super::BeginPlay();

	MyController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// init widgets for displaying inventory data
	wPickupNotification = CreateWidget<UPickupNotification>(GetWorld(), PickupNotificationClass);
	wInventoryDisplay = CreateWidget<UInventoryDisplay>(GetWorld(), InventoryDisplayClass);
	wItemDisplay = CreateWidget<UItemDisplay>(GetWorld(), InventoryDisplayClass);
	wPlayerWarning = CreateWidget<UPlayerWarning>(GetWorld(), WarningClass);

	// add empty string to viewport
	// change message to display item name whenever an item is added to overlapped items stack
	// string is emptied when stack becomes empty
	wPickupNotification->DisplayMessage->SetText(FText::FromString(""));
	wPickupNotification->AddToViewport();
}