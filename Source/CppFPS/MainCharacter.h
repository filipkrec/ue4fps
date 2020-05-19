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
#include "Kismet/KismetSystemLibrary.h" 
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "MainCharacter.generated.h"

UCLASS()
class CPPFPS_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Values)
		int health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Values)
		int shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Values)
		int energy;

	UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> Player_Shield_Widget_Class;
	UUserWidget* Player_Shield_Widget;
	UProgressBar* ShieldBar;
	UProgressBar* HealthBar;
	UProgressBar* EnergyBar;
	UTextBlock* ClipText;
	UTextBlock* AmmoText;

	FTimerHandle RegenTimer;
	FTimerHandle FireTimer;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;
	AWeapon* Weapon;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool sprinting;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	int sprintIncrease;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	int ADSMoveSpeedDecrease;

	UPROPERTY(EditAnywhere, Category = "Sprint")
	int sprintIncreasePercentage = 30;

	bool dead;

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
	void FireStart();
	void FireStop();

	void ADSOn();
	void ADSOff();

	void UpdateAmmoText();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
