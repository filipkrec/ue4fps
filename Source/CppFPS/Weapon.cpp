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
	if (Mesh.Succeeded())
		WeaponMesh->SetSkeletalMesh(Mesh.Object);
	ammoMax = 230;
	clipMax = 30;
	clipCurrent = 30;
	ammoCurrent = 100;
	firingMode = FiringMode::Automatic;
	attackPerSecond = 8;
	range = 2000.0f;
	damage = 10;

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
		FRotator direction;
		FVector charVector;

		character->GetController()->GetPlayerViewPoint(charVector, direction);

		FVector MuzzleLocation = WeaponMesh->GetSocketLocation("AKSocket");
		if (ProjectileClass != nullptr)
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, direction, FActorSpawnParameters());
			if (Projectile != nullptr)
			{
				Projectile->damage = damage;
				//Projectile->damage = damage;
				FVector LaunchVector = direction.Vector() * range;
				FHitResult hitResult;
				FVector endPoint;
				if (GetWorld()->LineTraceSingleByChannel(hitResult, MuzzleLocation + direction.Vector() * 100, charVector + LaunchVector, ECC_Visibility, FCollisionQueryParams()))
				{
					if (hitResult.Actor->GetClass() == ProjectileClass.Get())
					{
						UKismetSystemLibrary::PrintString(GetWorld(), "BULLET HIT");
					}
					endPoint = hitResult.ImpactPoint;
				}
				else
					endPoint = hitResult.TraceEnd;

				UKismetSystemLibrary::PrintString(GetWorld(), "Impact point = " + hitResult.ImpactPoint.ToString());
				FVector LaunchDirection = endPoint - MuzzleLocation;

				LaunchDirection.Normalize();
				Projectile->FireInDirection(LaunchDirection);
				--clipCurrent;
			}
		}

		FTransform mflashTransform(direction, MuzzleLocation, FVector(0.1f, 0.1f, 0.1f));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleFlash, mflashTransform);
	}
}

void AWeapon::StartFiring(ACharacter* character)
{
	if (firingMode == FiringMode::Automatic)
	{
		Fire(character);
		GetWorld()->GetTimerManager().SetTimer(firingTimer, FTimerDelegate::CreateUObject(this, &AWeapon::Fire, character), 1.0f / attackPerSecond, true, 1.0f / attackPerSecond);
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


