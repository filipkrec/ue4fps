// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#define DEBUG_CUSTOM 1

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	TSubclassOf<UAnimInstance> AnimBp = ConstructorHelpers::FClassFinder<UAnimInstance>(TEXT("AnimBlueprint'/Game/ThirdPersonCPP/Character/SwatAnimationBlueprint'")).Class;

	GetMesh()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ThirdPersonCPP/Character/Swat.Swat'")).Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBp);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 200.0f;
	GetCharacterMovement()->AirControl = 0.0f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->AttachTo(GetMesh(), "head");
	followCamera->bUsePawnControlRotation = true;

	followCamera->SetRelativeLocation(FVector(0.0f, 2.0f, 15.0f));
	followCamera->SetRelativeRotation(FRotator(80.0f, -90.0f, -180.0f));  // y , z , x

	ConstructorHelpers::FClassFinder<UUserWidget> BPShield(TEXT("/Game/ThirdPersonCPP/Blueprints/Shield_UI"));
	Player_Shield_Widget_Class = BPShield.Class;

	WeaponClass = ConstructorHelpers::FClassFinder<AWeapon>(TEXT("Class'/Script/CppFPS.Weapon'")).Class;

	sprintIncreasePercentage = 30;
	sprintIncrease = (GetCharacterMovement()->MaxWalkSpeed / 100) * sprintIncreasePercentage;

	ADSMoveSpeedDecrease = 300;

	AddState(playerState::STILL);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Player_Shield_Widget_Class != nullptr)
	{
		Player_Shield_Widget = CreateWidget(GetWorld(), Player_Shield_Widget_Class);
		Player_Shield_Widget->AddToViewport();
		ShieldBar = Player_Shield_Widget->WidgetTree->FindWidget<UProgressBar>("Shield");
		HealthBar = Player_Shield_Widget->WidgetTree->FindWidget<UProgressBar>("Health");
		EnergyBar = Player_Shield_Widget->WidgetTree->FindWidget<UProgressBar>("Energy");
		ClipText = Player_Shield_Widget->WidgetTree->FindWidget<UTextBlock>("Clip");
		AmmoText = Player_Shield_Widget->WidgetTree->FindWidget<UTextBlock>("Ammo");
		Crosshairs = Player_Shield_Widget->WidgetTree->FindWidget<UImage>("Crosshairs");
	}

	if (!CheckState(playerState::DEAD))
	{
		GetWorldTimerManager().SetTimer(RegenTimer, this, &AMainCharacter::RegenShield, 0.2, true, 0.0f);
	}

	health = 100;
	HealthBar->SetPercent(1.0f);
	shield = 100;
	ShieldBar->SetPercent(1.0f);
	energy = 100;
	EnergyBar->SetPercent(1.0f);
	UpdateAmmoText();

	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);

	if (WeaponClass != nullptr)
	{
		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		Weapon->AttachToComponent(GetMesh(), rules, "Weapon_attach");
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCharacterMovement()->Velocity == FVector(0.0f))
		AddState(playerState::STILL);
	HealthBar->SetPercent(FMath::Clamp((float)health / 100.0f, 0.0f, 1.0f));
	ShieldBar->SetPercent(FMath::Clamp((float)shield / 100.0f, 0.0f, 1.0f));
	UpdateAmmoText();
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("TakeDamage", IE_Pressed, this, &AMainCharacter::TakeDamage);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::FireStart);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMainCharacter::FireStop);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainCharacter::Reload);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::EndSprint);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &AMainCharacter::ADSOn);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &AMainCharacter::ADSOff);
}

void AMainCharacter::Jump()
{
	if (!CheckState(playerState::DEAD))
	{
		if (!CheckState(playerState::CROUCHING))
			Super::Jump();
		else
			ToggleCrouch();
	}
}


void AMainCharacter::ToggleCrouch()
{
	if (!CheckState(playerState::DEAD))
	{
		if (GetMovementComponent()->IsCrouching())
		{
			Super::UnCrouch();
			ClearState(playerState::CROUCHING);
		}
		else
		{
			Super::Crouch();
			AddState(playerState::CROUCHING);
			EndSprint();
		}
	}
}


void AMainCharacter::Sprint()
{
	if (!CheckState(playerState::DEAD) && !CheckState(playerState::SPRINTING))
	{
		FireStop();
		if(CheckState(playerState::CROUCHING))
		{
			ToggleCrouch();
		}
		AddState(playerState::SPRINTING);
		GetCharacterMovement()->MaxWalkSpeed += sprintIncrease;
	}
}

void AMainCharacter::EndSprint()
{
	if (CheckState(playerState::SPRINTING))
	{
		GetCharacterMovement()->MaxWalkSpeed -= sprintIncrease;
	}
	ClearState(playerState::SPRINTING);
}

void AMainCharacter::MoveForward(float Axis)
{
	if (!CheckState(playerState::DEAD))
	{
		if (CheckState(playerState::STILL))
		{
			ClearState(playerState::STILL);
			AddState(playerState::WALKING);
		}
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Axis);
	}
}

void AMainCharacter::MoveRight(float Axis)
{
	if (!CheckState(playerState::DEAD))
	{
		if(CheckState(playerState::STILL))
		{
			ClearState(playerState::STILL);
			AddState(playerState::WALKING);
		}
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Axis);
	}
}

void AMainCharacter::Die()
{
	ClearStates();
	AddState(playerState::DEAD);
	GetWorld()->GetTimerManager().ClearTimer(RegenTimer);
}

void AMainCharacter::TakeDamage()
{
	int value = 15;
	if (!CheckState(playerState::DEAD))
	{
		int dmgHealth = value;
		int dmgShield = value - value * (1.0 - 0.8); //ARMOR PERC REDUCE
		dmgShield = dmgShield < shield ? dmgShield : shield;
		dmgHealth = dmgHealth - dmgShield;
		shield -= dmgShield;
		health -= dmgHealth;

		if (health < 0)
		{
			Die();
		}
	}
}

void AMainCharacter::RegenShield()
{
	if (shield < 100)
		shield += 1;
}

void AMainCharacter::FireStart()
{
	if (!CheckState(playerState::SPRINTING) && !CheckState(playerState::RELOADING))
		Weapon->StartFiring(this);
}

void AMainCharacter::FireStop()
{
	if (Weapon != nullptr)
	{
		Weapon->StopFiring(this);
	}
}

void AMainCharacter::Reload()
{
	FireStop();
	if (Weapon != nullptr && Weapon->ammoCurrent > 0 && (Weapon->clipCurrent != Weapon->clipMax) && !CheckState(playerState::SPRINTING))
	{
		AddState(playerState::RELOADING);
		GetWorldTimerManager().SetTimer(ReloadingTimer, this, &AMainCharacter::Reloaded, 2.4f, false, 2.4f);
	}
}

void AMainCharacter::Reloaded()
{
	Weapon->Reload();
	ClearState(playerState::RELOADING);
	GetWorldTimerManager().ClearTimer(ReloadingTimer);
}


void AMainCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == "sprintIncrease")
		sprintIncreasePercentage = (float)sprintIncrease / GetCharacterMovement()->MaxWalkSpeed * 100;
	else if (PropertyChangedEvent.GetPropertyName() == "sprintIncreasePercentage")
		sprintIncrease = GetCharacterMovement()->MaxWalkSpeed / 100 * sprintIncreasePercentage;

}


void AMainCharacter::UpdateAmmoText()
{
	if (Weapon != nullptr)
	{
		AmmoText->Font.Size = 30;
		AmmoText->SetText(FText::FromString(FString::FromInt(Weapon->ammoCurrent) + "/" + FString::FromInt(Weapon->ammoMax)));
		ClipText->Font.Size = 30;
		ClipText->SetText(FText::FromString(FString::FromInt(Weapon->clipCurrent) + "/" + FString::FromInt(Weapon->clipMax)));
	}
}


void AMainCharacter::ADSOn()
{
	if (Weapon != nullptr && !CheckState(playerState::SPRINTING) && !CheckState(playerState::RELOADING))
	{
		AddState(playerState::AIMING);
		followCamera->Deactivate();
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(Weapon);
		if (Crosshairs)
		Crosshairs->SetVisibility(ESlateVisibility::Hidden);
		Weapon->ADSCamera->Activate();
		GetCharacterMovement()->MaxWalkSpeed -= ADSMoveSpeedDecrease;
	}
}

void AMainCharacter::ADSOff()
{
	ClearState(playerState::AIMING);
	Weapon->ADSCamera->Deactivate();
	GetController()->CastToPlayerController()->SetViewTarget(this);
	if(Crosshairs)
	Crosshairs->SetVisibility(ESlateVisibility::Visible);
	followCamera->Activate();
	GetCharacterMovement()->MaxWalkSpeed += ADSMoveSpeedDecrease;
}



void AMainCharacter::AddState(playerState state)
{
	if(states.Contains(state))
	{
		return;
	}
	else
	{
		states.Add(state);
	}
}

bool AMainCharacter::CheckState(const playerState& state)
{
	return states.Contains(state);
}

void AMainCharacter::ClearState(const playerState& state)
{
	states.RemoveAll([&](const playerState& val) {
		return val == state;
		});
}

void AMainCharacter::ClearStates()
{
	states.Empty();
}