// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h" 
#include "GameFramework/ProjectileMovementComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Projectile_AK47.generated.h"

UCLASS()
class CPPFPS_API AProjectile_AK47 : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AProjectile_AK47();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Collision comp
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = Movement)
		UStaticMeshComponent* StaticMesh;

	void FireInDirection(const FVector& ShootDirection);

};
