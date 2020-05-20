// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Projectile.h"
#include "DualVector.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "GenericPlatform/GenericPlatformMath.h"
#include "Camera/CameraComponent.h" 	
#include "Particles/ParticleSystem.h"

#include "Weapon.generated.h"

UCLASS()
class CPPFPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	enum class FiringMode {
		Single,
		Automatic,
		Burst
	};

	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
		float damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
		int attackPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
		float fireVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
		float spread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int clipMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int ammoMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int clipCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int ammoCurrent;

	FiringMode firingMode;
	FTimerHandle firingTimer;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		UCameraComponent* ADSCamera;

	TSubclassOf<UParticleSystem> muzzleFlashClass;
	UParticleSystem* muzzleFlash;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Fire(ACharacter* character);

	void StartFiring(ACharacter* character);
	void StopFiring(ACharacter* character);
	void Reload();
};
