// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

//Engine libraries
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
//Custom libraries
#include "Interfaces/LockOn_Interface.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Controller = GetWorld()->GetFirstPlayerController();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlatformer3DCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlatformer3DCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlatformer3DCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlatformer3DCharacter::LookUpAtRate);

	/** MY BEHAVIOUR **/
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlatformer3DCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlatformer3DCharacter::EndJump);

	PlayerInputComponent->BindAxis("ZoomCamera", this, &APlayerCharacter::ZoomCamera);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlatformer3DCharacter::StartDash);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlatformer3DCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlatformer3DCharacter::EndRun);

	PlayerInputComponent->BindAction("ToggleCameraMode", IE_Pressed, this, &APlayerCharacter::ToggleCameraMode);

	PlayerInputComponent->BindAction("RollDodge", IE_Pressed, this, &APlayerCharacter::RollDodgeInput);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlatformer3DCharacter::StartAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &APlatformer3DCharacter::StartHeavyAttack);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlatformer3DCharacter::ToggleCrouchState);
}

void APlayerCharacter::LookAt(FVector Location, float Rate)
{
	FRotator ControlRotation = Controller->GetControlRotation();

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), Location);
	ControlRotation.Yaw = FMath::RInterpTo(ControlRotation, TargetRotation, Rate, 0.0f).Yaw;
	Controller->SetControlRotation(ControlRotation);
}

void APlayerCharacter::ZoomCamera(float Value)
{
	float newDistance = CameraBoom->TargetArmLength -= Value * CameraZoomSpeed;
	CameraBoom->TargetArmLength = FMath::Clamp(newDistance, CameraZoomMinMax.X, CameraZoomMinMax.Y);
}

void APlayerCharacter::ToggleCameraMode()
{
	switch (CameraMode)
	{
	case 0:
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
		CameraMode = 1;
		FindNearestTarget();
		break;
	case 1:
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;
		CameraMode = 0;
		LockOffTarget();
		break;
	}
}

void APlayerCharacter::RollDodgeInput(FKey Key)
{
	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	// If Left Alt key or B button were pressed
	if (Key == EKeys::X || Key == EKeys::Gamepad_FaceButton_Right)
	{
		RollDodge();
	}
	else
	{
		// If wanted to prevent action by setting a direction only AFTER pressing the action key, clear timer here.
		KeyBuffer = Key;
	}
}

void APlayerCharacter::RollDodge()
{
	if (RollDodgeAction < 2 && RollDodgeAnimation == 0)
	{
		RollDodgeAction++;
		GetWorldTimerManager().ClearTimer(RollTimerHandle);
		GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APlayerCharacter::ExecuteRollDodge, 0.17f, false);
	}
}

void APlayerCharacter::ExecuteRollDodge()
{
	//If character is doing a blocking animation, they can´t roll/dodge (possible to make this check on RollDodge() to prevent creation of unnecessary timers).
	if (GetCharacterMovement()->IsFalling() || AttackSystem->IsAttackAnimation())
	{
		ResetRollDodgeAnimation();
		return;
	}

	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	FKey KeyNone;

	if (RollDodgeAction >= 2)
	{
		RollDodgeAction = 2;
	}

	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	// If Left Alt key or B button were pressed
	if (!Cast<APlayerController>(Controller)->IsInputKeyDown(KeyBuffer))
	{
		KeyBuffer = KeyNone;
	}

	ResetMoveState();

	// Little hack to set animation index to 0 or 4, to offset wether action is a roll or a dodge
	// Followed by a galaxy brain hack, if there is no direction pressed make animation index 0 so no animation will be played
	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	RollDodgeAnimation = ((RollDodgeAction - 1) * 4) * (KeyBuffer != KeyNone);

	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	if (CameraMode == 0)
	{
		if (KeyBuffer != KeyNone)
		{
			RollDodgeAnimation += 1;
		}
	}
	else
	{
		if (KeyBuffer == EKeys::W || KeyBuffer == EKeys::Gamepad_LeftStick_Up)
		{
			RollDodgeAnimation += 1;
		}
		else if (KeyBuffer == EKeys::A || KeyBuffer == EKeys::Gamepad_LeftStick_Left)
		{
			RollDodgeAnimation += 2;
		}
		else if (KeyBuffer == EKeys::S || KeyBuffer == EKeys::Gamepad_LeftStick_Down)
		{
			RollDodgeAnimation += 3;
		}
		else if (KeyBuffer == EKeys::D || KeyBuffer == EKeys::Gamepad_LeftStick_Right)
		{
			RollDodgeAnimation += 4;
		}
	}

	float ResetTimer = RollDodgeAnimation <= 4 ? 0.7 : 0.3;
	GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APlatformer3DCharacter::ResetRollDodgeAnimation, ResetTimer, false);
}

void APlayerCharacter::FindNearestTarget()
{
	if (!TargetLocked)
	{
		float ClosestTargetDistance = MinimumDistanceToTarget;

		UWorld* World = GetWorld();
		/*** Pending: Use: UGameplayStatics::GetAllActorsWithInterface() instead of iterator ***/
		TActorIterator<AActor> It(World);
		FHitResult HitResult;

		for (It; It; ++It)
		{
			if ((*It)->GetClass()->ImplementsInterface(ULockOn_Interface::StaticClass()))
			{
				/***** If you're using a BP interface, you need to use ScriptInterfaceCast instead of an InterfaceCast.
						...
						However, the best way to use interfaces is with the IMyInterface::Execute_MyFunction,
						because that handles both the native and BP cases via the same path.
						https://answers.unrealengine.com/questions/43038/buginerfacecast-returns-null-for-blueprint-classes.html
				*****/
				// ILockOn_Interface* Target = Cast<ILockOn_Interface>(*It);

				float DistanceToTarget = GetDistanceTo(*It);
				if (DistanceToTarget < ClosestTargetDistance)
				{
					FCollisionQueryParams TraceParameters(FName(TEXT("")), false, this);

					if (World->LineTraceSingleByChannel(HitResult, GetActorLocation(), (*It)->GetActorLocation(), ECC_PhysicsBody, TraceParameters))
					{
						if (HitResult.Actor.Get() == *It)
						{
							ClosestTargetDistance = DistanceToTarget;
							LockOnTarget(*It);
						}
					}
				}
			}
		}
	}
}

void APlayerCharacter::ReactToDamage(float AttackForce)
{
	Super::ReactToDamage(AttackForce);

	if (!HealthComponent->IsAlive())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}