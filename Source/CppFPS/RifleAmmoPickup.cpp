// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleAmmoPickup.h"

// Sets default values
ARifleAmmoPickup::ARifleAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (Mesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(Mesh.Object);
		ConstructorHelpers::FObjectFinder<UMaterial>material(TEXT("/Game/StarterContent/Materials/M_Metal_Burnished_Steel.M_Metal_Burnished_Steel"));
		StaticMesh->SetMaterial(0, (UMaterialInterface*)material.Object);
		StaticMesh->SetupAttachment(RootComponent);

		StaticMesh->SetCollisionProfileName("OverlapAll");
		StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ARifleAmmoPickup::OnBeginOverlap);

		//FVector Scale;
		//Scale.X = 0.07f;
		//Scale.Y = 0.07f;
		//Scale.Z = 0.07f;

		//FVector Position;
		//Position.X = 0.0f;
		//Position.Y = 0.0f;
		//Position.Z = -3.5f;

		//StaticMesh->SetRelativeLocation(Position);
		//StaticMesh->SetRelativeScale3D(Scale);

		ammoAmount = 30;
	}

}

// Called when the game starts or when spawned
void ARifleAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARifleAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARifleAmmoPickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->IsChildOf<AMainCharacter>())
	{
		AMainCharacter* character = Cast<AMainCharacter>(OtherActor);
		_ASSERT(character != nullptr);
		if(character->Weapon != nullptr) 
		{
			int &ammoCurrent = character->Weapon->ammoCurrent;
			int &ammoMax = character->Weapon->ammoMax;
			ammoCurrent = ammoCurrent + ammoAmount > ammoMax ? ammoMax : ammoCurrent + ammoAmount;

			UKismetSystemLibrary::PrintString(GetWorld(), "Ammo filled");

			this->Destroy();
		}
	}
}

