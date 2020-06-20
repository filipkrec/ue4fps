// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetTree.h"
#include "Weapon.h"
#include "Projectile_AK47.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class playerState : uint8
{
	STILL UMETA(DisplayName = "Still"),
	WALKING UMETA(DisplayName = "Walking"),
	SPRINTING UMETA(DisplayName = "Sprinting"),
	CROUCHING UMETA(DisplayName = "Crouching"),
	AIMING UMETA(DisplayName = "Aiming"),
	SHOOTING UMETA(DisplayName = "Shooting"),
	RELOADING UMETA(DisplayName = "Reloading"),
	DEAD UMETA(DisplayName = "Dead")
};

UCLASS()
class CPPFPS_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();
	//COMPONENTS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		UCameraComponent* followCamera;

	//ENERGY VALUES
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Values)
		int health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Values)
		int shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Values)
		int energy;

	//WIDGETS
	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> Player_Shield_Widget_Class;
	UUserWidget* Player_Shield_Widget;
	UProgressBar* ShieldBar;
	UProgressBar* HealthBar;
	UProgressBar* EnergyBar;
	UTextBlock* ClipText;
	UTextBlock* AmmoText;

	//TIMERS
	FTimerHandle RegenTimer;
	FTimerHandle FireTimer;
	FTimerHandle ReloadingTimer;

	//WEAPON
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		TSubclassOf<AWeapon> WeaponClass;
	AWeapon* Weapon;

	//MOVEMENT
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		int sprintIncrease;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		int ADSMoveSpeedDecrease;

	UPROPERTY(EditAnywhere, Category = "Sprint")
		int sprintIncreasePercentage = 30;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool reloading;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		bool sprinting;

	//STATES
	TArray<playerState> states;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void TakeDamage();
	void RegenShield();

	void Jump() override;
	void ToggleCrouch();
	void Sprint();
	void EndSprint();

	void Die();

	void Reload();
	void Reloaded();
	void FireStart();
	void FireStop();

	void ADSOn();
	void ADSOff();

	void UpdateAmmoText();

	UFUNCTION(BlueprintCallable)
	void AddState(playerState state);

	UFUNCTION(BlueprintCallable)
	bool CheckState(const playerState& state);

	UFUNCTION(BlueprintCallable)
	void ClearState(const playerState& state);

	void ClearStates();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
