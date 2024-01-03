// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "MyActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "MyActorComponent.h"
#include "EnhancedInputComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Box);

	Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left"));
	Left->SetupAttachment(Body);

	Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right"));
	Right->SetupAttachment(Body);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Box);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Box);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));

	RotateComponent = CreateDefaultSubobject<UMyActorComponent>(TEXT("Rotator"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Body(TEXT("/Script/Engine.StaticMesh'/Game/P38/Meshes/SM_P38_Body.SM_P38_Body'"));
	if (SM_Body.Succeeded())
	{
		Body->SetStaticMesh(SM_Body.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Propeller(TEXT("/Script/Engine.StaticMesh'/Game/P38/Meshes/SM_P38_Propeller.SM_P38_Propeller'"));
	if (SM_Propeller.Succeeded())
	{
		Left->SetStaticMesh(SM_Propeller.Object);
		Right->SetStaticMesh(SM_Propeller.Object);
	}

	Box->SetBoxExtent(FVector(51.0f, 69.0f, 18.0f));

	Left->AddLocalOffset(FVector(37.0f, -21.0f, 1.0f));
	Right->AddLocalOffset(FVector(37.0f, 21.0f, 1.0f));

	Arrow->AddLocalOffset(FVector(100.0f, 0.0f, 0.0f));

	SpringArm->TargetArmLength = 150.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	RotateComponent->Components.Add(Left);
	RotateComponent->Components.Add(Right);
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddMovementInput(GetActorForwardVector());
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIC && FireAction && PitchAndRollAction)
	{
		EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &AMyPawn::Fire);
		EIC->BindAction(PitchAndRollAction, ETriggerEvent::Triggered, this, &AMyPawn::PitchAndRoll);
	}
}

void AMyPawn::Fire(const FInputActionValue& Value)
{
	if (Projectile)
	{
		GetWorld()->SpawnActor<AActor>(Projectile, Arrow->GetComponentTransform());
	}
}

void AMyPawn::PitchAndRoll(const FInputActionValue& Value)
{
	FVector2D RotationValue = Value.Get<FVector2D>();
	if (RotationValue.IsZero()) return;
	FVector2D DeltaRotaionValue = RotationValue * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * 60;
	AddActorLocalRotation(FRotator(DeltaRotaionValue.Y, 0, DeltaRotaionValue.X));
}

