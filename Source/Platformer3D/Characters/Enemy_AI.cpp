// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_AI.h"

// Engine libraries
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
// Custom libraries

// Sets default values
AEnemy_AI::AEnemy_AI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting MinMax Distance to player
	DistanceToPlayerMinMax = FVector2D(200.f, 1000.f);

	// Create Pawn Sensing Component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensor"));
	PawnSensingComponent->SetPeripheralVisionAngle(90.f);
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy_AI::OnSeePawn);

	USceneComponent* MeshComponent = Cast<USceneComponent>(GetMesh());
	// Create GUI Widget Component
	GUIWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("GUI Component"));

	AssassinationArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Assassination Area"));

	if (MeshComponent)
	{
		GUIWidgetComponent->SetupAttachment(MeshComponent);
		AssassinationArea->SetupAttachment(MeshComponent);
	}
}

// Called when the game starts or when spawned
void AEnemy_AI::BeginPlay()
{
	Super::BeginPlay();
	
	//***** This is just a reminder to remember how to check if a given object implements an Interface *****//
	//***** Syntax is: MyObject->GetClass()->ImplementsInterface(MyInterface::StaticClass())  *****//
	//***** ¿¿¿MyInterface MUST be an interface class???  *****//
	//this->GetClass()->ImplementsInterface(AEnemy_AI::StaticClass());

	if (GUIWidgetComponent)
	{
		GUIWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.f));
		GUIWidget = Cast<UEnemyGUI_Widget>(GUIWidgetComponent->GetUserWidgetObject());
		if (GUIWidget)
		{
			//***** Widget is showing way below the character, despite setting it's position in the blueprint's vieport tab, maybe set a default position *****//
			GUIWidget->SetOwnerNPC(this);
		}
	}

	PawnSensingComponent->SightRadius = DistanceToPlayerMinMax.Y;

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Cast<AAIController>(Controller)->bSetControlRotationFromPawnOrientation = 0;
}

// Called every frame
void AEnemy_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GUIWidgetComponent)
	{
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetFollowCamera()->GetComponentLocation());
		GUIWidgetComponent->SetWorldRotation(TargetRotation);
	}

	if (HealthComponent->IsAlive() && !IsStunned)
	{
		if (!IsWaiting)
		{
			if (SeenPlayer)
			{
				SeekPlayer();
			}
			else if (TargetPoints.Num() > 0)
			{
				MoveToTargetPoint();
			}
		}
		else
		{
			if (WaitTimer > 0.f)
			{
				WaitTimer -= DeltaTime;
				IsWaiting = WaitTimer > 0.f;
			}
		}
	}
}

// Called to bind functionality to input
void AEnemy_AI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/** MY BEHAVIOUR **/
void AEnemy_AI::OnSeePawn(APawn* OtherPawn)
{
	if (!SeenPlayer && OtherPawn == PlayerCharacter && HealthComponent->IsAlive())
	{
		CancelWait();
		SeenPlayer = true;
		if (!NearestTarget)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
			LockOnTarget(OtherPawn);
		}
	}
}

void AEnemy_AI::SeekPlayer()
{
	if (HealthComponent->IsAlive() && !AttackSystem->IsAttackAnimation() && !IsFlinching)
	{
		if (DistanceToPlayer() >= DistanceToPlayerMinMax.X)
		{
			if (AttackSystem->CanRangedAttack())
			{
				float Action = FMath::RandRange(1.f, 10.f);

				if (Action >= 9.f)
				{
					FVector Location = WeaponMesh ? WeaponMesh->GetComponentLocation() : GetActorLocation();
					//FRotator Rotation = ArrowMesh ? ArrowMesh->GetComponentRotation() : GetActorRotation() ;
					FRotator Rotation = GetActorRotation();
					AttackSystem->FireRangedAttack(Location, Rotation);
					Wait(1.f);
					return;
				}
			}

			//Cast<AAIController>(Controller)->MoveToActor(PlayerCharacter, 5.f);
			MoveForward(1.0);
		}
		else if (!GetCharacterMovement()->IsFalling())
		{
			/***** For now, enemies can't attack on air, so i do this check here to prevent leaving state unchanged when calling Start Attack*****/
			StartAttack();

			/*** Temporary Combat logic (Need refactor) ***/
			CurrentCombo = AttackSystem->GetCurrentCombo();
			if (CurrentCombo < AttackSystem->GetMaxCombo() - 1)
			{
				Wait(1.0);
			}
			else
			{
				Wait(2.f);
			}
		}
		
		if (DistanceToPlayer() > DistanceToPlayerMinMax.Y)
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bUseControllerRotationYaw = false;
			LockOffTarget();
			SeenPlayer = false;
		}
	}
}

void AEnemy_AI::DoDamage(AActor* Target)
{
	if (Target == PlayerCharacter)
	{
		float Damage = UKismetMathLibrary::RandomFloatInRange(BaseDamage, AttackSystem->GetAttackDamage());
		UGameplayStatics::ApplyDamage(Target, Damage, Controller, this, NULL);
	}
}

void AEnemy_AI::ReactToDamage(float AttackForce)
{
	Super::ReactToDamage(AttackForce);

	if (!HealthComponent->IsAlive())
	{
		SeenPlayer = false;

		PlayerCharacter->AddExp(ExpGiven);
	}
}

float AEnemy_AI::DistanceToPlayer()
{
	return GetDistanceTo(PlayerCharacter);
}

void AEnemy_AI::Wait(float Seconds)
{
	WaitTimer = Seconds;
	IsWaiting = true;
}

void AEnemy_AI::CancelWait()
{
	IsWaiting = false;
	WaitTimer = 0.0f;
}

void AEnemy_AI::MoveToTargetPoint()
{
	/*** Fix rotation and distance tolerance ***/
	if (FVector::DistXY(GetActorLocation(), TargetPoints[CurrentTargetPointIndex]->GetTargetLocation()) > TargetPointDistanceTolerance)
	{
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPoints[CurrentTargetPointIndex]->GetTargetLocation());
		FVector TargetDirection = TargetRotation.Vector();

		TargetDirection.Normalize();
		MoveForward(TargetDirection.X * 0.5f);
		MoveRight(TargetDirection.Y * 0.5f);
	}
	else
	{
		CurrentTargetPointIndex = (CurrentTargetPointIndex + 1) % TargetPoints.Num();
		Wait(5.f);
	}
}