
// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#define DEBUG_CUSTOM 1

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Use a sphere as a simple collision representation.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// Set the sphere's collision radius.
	CollisionComponent->InitSphereRadius(3.5f);
	CollisionComponent->SetCollisionProfileName("NoTrace");
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->bVisibleInRayTracing = false;

	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	// Set the root component to be the collision component.
	RootComponent = CollisionComponent;

	//Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (Mesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(Mesh.Object);
		ConstructorHelpers::FObjectFinder<UMaterial>material(TEXT("/Game/StarterContent/Materials/M_Metal_Burnished_Steel.M_Metal_Burnished_Steel"));
		StaticMesh->SetMaterial(0, (UMaterialInterface*)material.Object);
		StaticMesh->SetupAttachment(RootComponent);
		StaticMesh->SetCollisionProfileName("NoTrace");

		FVector Scale;
		Scale.X = 0.07f;
		Scale.Y = 0.07f;
		Scale.Z = 0.07f;

		FVector Position;
		Position.X = 0.0f;
		Position.Y = 0.0f;
		Position.Z = -3.5f;

		StaticMesh->SetRelativeLocation(Position);
		StaticMesh->SetRelativeScale3D(Scale);
	}

	// Projectile Movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 4000.0f;
	ProjectileMovementComponent->MaxSpeed = 4000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.1f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.3f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor->GetClass()->IsChildOf<ASimpleAI>())
	{
		ASimpleAI* character = Cast<ASimpleAI>(OtherActor);
		_ASSERT(character != nullptr);
		if (character->health > 0)
		{
			character->health -= damage;
#ifdef DEBUG_CUSTOM
			UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(character->health));
#endif
		}
	}

	Destroy();
}

