// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AK
	ProjectileClass = ConstructorHelpers::FClassFinder<AProjectile>(TEXT("Class'/Script/CppFPS.Projectile'")).Class;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>Mesh(TEXT("SkeletalMesh'/Game/ThirdPersonCPP/Weapons/AK47/AK.AK'"));
	if(Mesh.Succeeded())
		WeaponMesh->SetSkeletalMesh(Mesh.Object);
	ammoMax = 230;
	clipMax = 30;
	clipCurrent = 30;
	ammoCurrent = 100;
	firingMode = FiringMode::Automatic;
	attackPerSecond = 8;

	ADSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ADSCamera"));
	ADSCamera->bUsePawnControlRotation = true;
	ADSCamera->AttachTo(WeaponMesh);
	ADSCamera->SetRelativeLocation(FVector(-0.098, -45.17f, -118.265f));
	ADSCamera->SetRelativeRotation(FRotator(90.0f, 44.0f, -44.0f));  //Y Z X

	ConstructorHelpers::FObjectFinder<UParticleSystem>PSystem(TEXT("ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Chicken/P_ChickenMaster_MuzzleFlash_01.P_ChickenMaster_MuzzleFlash_01'"));
	if (PSystem.Succeeded())
		muzzleFlash = PSystem.Object;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeapon::Fire(ACharacter* character)
{
	if (WeaponMesh != nullptr && clipCurrent != 0)
	{
		FRotator direction = character->GetViewRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = character;
		SpawnParams.Instigator = Instigator;

		FVector MuzzleLocation = WeaponMesh->GetSocketLocation("AKSocket");
		if (ProjectileClass != nullptr)
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, direction, SpawnParams);
			if (Projectile != nullptr)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = direction.Vector();
				Projectile->FireInDirection(LaunchDirection);
				--clipCurrent;
			}
		}
		FTransform mflashTransform(direction, MuzzleLocation, FVector(0.1f, 0.1f, 0.1f));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),muzzleFlash, mflashTransform);
	}
}

void AWeapon::StartFiring(ACharacter* character)
{
	if (firingMode == FiringMode::Automatic)
	{
		Fire(character);
		GetWorld()->GetTimerManager().SetTimer(firingTimer, FTimerDelegate::CreateUObject(this, &AWeapon::Fire, character), 1.0f / attackPerSecond, true,  1.0f / attackPerSecond);
	}
	else if (firingMode == FiringMode::Single)
	{
		Fire(character);
	}
}

void AWeapon::StopFiring(ACharacter* character)
{
	GetWorld()->GetTimerManager().ClearTimer(firingTimer);
}

void AWeapon::Reload()
{
	if (WeaponMesh != nullptr && clipCurrent != clipMax && ammoCurrent != 0)
	{
		int emptyClipSlots = clipMax - clipCurrent;
		int delta = FGenericPlatformMath::Min(emptyClipSlots, ammoCurrent);
		ammoCurrent -= delta;
		clipCurrent += delta;
	}
}


