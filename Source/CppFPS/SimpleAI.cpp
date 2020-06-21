// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleAI.h"
#define DEBUG_CUSTOM 1

// Sets default values
ASimpleAI::ASimpleAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	health = 200;

	TSubclassOf<UAnimInstance> AnimBp = ConstructorHelpers::FClassFinder<UAnimInstance>(TEXT("/Game/Mannequin/Animations/ThirdPersonAnim")).Class;
	ConstructorHelpers::FObjectFinder<USkeletalMesh>SkeletalMesh(TEXT("/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin"));

	if (SkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -86.5f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(AnimBp);
	}
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASimpleAI::OnBeginOverlap);

	AIControllerClass = AAIController::StaticClass();

	pawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensor"));
	pawnSensing->SensingInterval = 0.5f;
	pawnSensing->bOnlySensePlayers = false;
	pawnSensing->SightRadius = 1300.0f;
	pawnSensing->SetPeripheralVisionAngle(40.0f);
	pawnSensing->OnSeePawn.AddDynamic(this, &ASimpleAI::OnSeePawn);
}

// Called when the game starts or when spawned
void ASimpleAI::BeginPlay()
{
	Super::BeginPlay();
	moveController = Cast<AAIController>(GetController());

	GetWorld()->GetTimerManager().SetTimer(patrolMoveHandle, this, &ASimpleAI::moveToRandomReachable, 3.0f, true);
}

// Called every frame
void ASimpleAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASimpleAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}

void ASimpleAI::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ASimpleAI::OnSeePawn(APawn* OtherPawn)
{
	this->moveController->MoveToActor(OtherPawn);
	//GetWorld()->GetTimerManager().ClearTimer(patrolMoveHandle);
}


void ASimpleAI::Alert(APawn* otherPawn)
{
	this->moveController->MoveToActor(otherPawn);
	//GetWorld()->GetTimerManager().ClearTimer(patrolMoveHandle);
}


void ASimpleAI::moveToRandomReachable()
{
	FVector reachableLoc = UNavigationSystemV1::GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), randomLocRadius);

#ifdef DEBUG_CUSTOM
	UKismetSystemLibrary::PrintString(GetWorld(),reachableLoc.ToString());
#endif
	
	moveController->MoveToLocation(reachableLoc);
}

