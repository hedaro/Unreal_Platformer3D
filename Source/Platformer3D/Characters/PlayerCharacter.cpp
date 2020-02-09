// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

//Engine libraries
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Platformer3DPlayerController.h"
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

	if (SkillsMenuWidget)
	{
		SkillsMenu = CreateWidget<UUserWidget>(Cast<APlayerController>(Controller), SkillsMenuWidget, TEXT("Skills Menu"));
		//SkillsMenu->AddToViewport();
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttackSystem->IsAiming() && FollowCamera->FieldOfView > 40.f)
	{
		FollowCamera->FieldOfView = FMath::Lerp(40.f, 90.f, 0.4f * DeltaTime);
	}
	else if (FollowCamera->FieldOfView < 90.f)
	{
		FollowCamera->FieldOfView = FMath::Lerp(90.f, 40.f, 0.4f * DeltaTime);
	}
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
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::StartDash);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlatformer3DCharacter::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlatformer3DCharacter::EndRun);

	PlayerInputComponent->BindAction("ToggleCameraMode", IE_Pressed, this, &APlayerCharacter::ToggleCameraMode);

	PlayerInputComponent->BindAction("RollDodge", IE_Pressed, this, &APlayerCharacter::RollDodgeInput);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlatformer3DCharacter::StartAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &APlayerCharacter::StartHeavyAttack);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlatformer3DCharacter::ToggleCrouchState);

	PlayerInputComponent->BindAction("SkillsMenu", IE_Pressed, this, &APlayerCharacter::ToggleSkillsMenu);

	PlayerInputComponent->BindAction("SaveGame", IE_Pressed, this, &APlayerCharacter::SaveGame);
	PlayerInputComponent->BindAction("LoadGame", IE_Pressed, this, &APlayerCharacter::LoadGame);

	PlayerInputComponent->BindAction("QuickItem", IE_Pressed, this, &APlayerCharacter::UseHealthPotion);

	PlayerInputComponent->BindAction("Bow", IE_Pressed, this, &APlayerCharacter::StartRangedAttack);
	PlayerInputComponent->BindAction("Bow", IE_Released, this, &APlayerCharacter::EndRangedAttack);
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
		if (AttackSystem->IsAiming())
		{
			return;
		}
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

void APlayerCharacter::StartRangedAttack()
{
	if (AttackSystem->CanAttack() && ItemsHeld.Contains(EItem_Types::IT_Arrow) && ItemsHeld[EItem_Types::IT_Arrow] > 0)
	{
		ShowRangedWeaponMeshes();
		AttackSystem->AimRangedAttack();

		if (CameraMode == 0)
		{
			ToggleCameraMode();
		}
	}
}

void APlayerCharacter::EndRangedAttack()
{
	if (AttackSystem->IsAiming() && ItemsHeld.Contains(EItem_Types::IT_Arrow) && ItemsHeld[EItem_Types::IT_Arrow] > 0)
	{
		HideRangedWeaponMeshes();

		FVector Location = ArrowMesh ? ArrowMesh->GetComponentLocation() : GetActorLocation() ;
		//FRotator Rotation = ArrowMesh ? ArrowMesh->GetComponentRotation() : GetActorRotation() ;
		FRotator Rotation = GetActorRotation() ;
		AttackSystem->FireRangedAttack(Location, Rotation);

		ItemsHeld[EItem_Types::IT_Arrow] = ItemsHeld[EItem_Types::IT_Arrow] - 1;

		if (CameraMode == 1 && !TargetLocked)
		{
			ToggleCameraMode();
		}
	}
}

void APlayerCharacter::CancelAttack()
{
	Super::CancelAttack();
	HideRangedWeaponMeshes();
	FollowCamera->FieldOfView = 90.f;
}

void APlayerCharacter::RegisterWeaponSheathMesh(UStaticMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		SwordSheathMesh = MeshComponent;
		SwordSheathMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		SwordSheathMesh->SetCollisionProfileName(TEXT("NoCollision"));
		SwordSheathMesh->SetHiddenInGame(true);
	}
}

void APlayerCharacter::RegisterBowMesh(UStaticMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		BowMesh = MeshComponent;
		BowMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		BowMesh->SetCollisionProfileName(TEXT("NoCollision"));
		BowMesh->SetHiddenInGame(true);
	}
}

void APlayerCharacter::RegisterBowSheathMesh(UStaticMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		BowSheathMesh = MeshComponent;
		BowSheathMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		BowSheathMesh->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void APlayerCharacter::RegisterArrowMesh(UStaticMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		ArrowMesh = MeshComponent;
		ArrowMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		ArrowMesh->SetCollisionProfileName(TEXT("NoCollision"));
		ArrowMesh->SetHiddenInGame(true);
	}
}

void APlayerCharacter::ShowRangedWeaponMeshes()
{
	WeaponMesh->SetHiddenInGame(true);
	BowSheathMesh->SetHiddenInGame(true);
	SwordSheathMesh->SetHiddenInGame(false);
	BowMesh->SetHiddenInGame(false);
	ArrowMesh->SetHiddenInGame(false);
}

void APlayerCharacter::HideRangedWeaponMeshes()
{
	WeaponMesh->SetHiddenInGame(false);
	BowSheathMesh->SetHiddenInGame(false);
	SwordSheathMesh->SetHiddenInGame(true);
	BowMesh->SetHiddenInGame(true);
	ArrowMesh->SetHiddenInGame(true);
}

void APlayerCharacter::AddExp(float Experience)
{
	if (CurrentExp + Experience >= ExpToNextLevel)
	{
		CurrentExp += Experience - ExpToNextLevel;
		ExpToNextLevel *= 2.f;
		LevelUp();
	}
	else
	{
		CurrentExp += Experience;
	}
}

void APlayerCharacter::LevelUp()
{
	PlayerLevel++;
	SkillPoints++;
}

float APlayerCharacter::GetCurrentExp() const
{
	return CurrentExp;
}

float APlayerCharacter::GetExpToNextLevel() const
{
	return ExpToNextLevel;
}

int APlayerCharacter::GetCurrentLevel() const
{
	return PlayerLevel;
}

void APlayerCharacter::ToggleSkillsMenu()
{
	if (SkillsMenu)
	{
		if (SkillsMenu->IsInViewport())
		{
			HideSkillsMenu();
		}
		else
		{
			ShowSkillsMenu();
		}
	}
}

void APlayerCharacter::ShowSkillsMenu()
{
	if (SkillsMenu)
	{
		SkillsMenu->AddToViewport();
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->bShowMouseCursor = true;
	}
}

void APlayerCharacter::HideSkillsMenu()
{
	if (SkillsMenu)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
		SkillsMenu->RemoveFromViewport();
	}
}

FSkill APlayerCharacter::GetSkill(FString SkillName) const
{
	const FSkill* Skill = Skills.Find(SkillName);

	if (Skill)
	{
		return *Skill;
	}

	return FSkill{ TEXT(""), 0, true };
}

int APlayerCharacter::SpendSkillPoints(int Amount)
{
	if (SkillPoints >= Amount && Amount > 0)
	{
		return SkillPoints -= Amount;
	}
	
	return SkillPoints;
}

int APlayerCharacter::GetSkillPoints() const
{
	return SkillPoints;
}

bool APlayerCharacter::IsSkillAvailable(FString SkillName) const
{
	const FSkill* Skill = Skills.Find(SkillName);

	if (!Skill)
	{
		return false;
	}
	else if (Skill->SkillRequired != TEXT(""))
	{
		const FSkill* SkillRequired = Skills.Find(Skill->SkillRequired);
		if (SkillRequired)
		{
			return SkillRequired->Acquired && !Skill->Acquired;
		}
	}

	return !Skill->Acquired;
}

void APlayerCharacter::AcquireSkill(FString SkillName)
{
	if (Skills.Contains(SkillName) && IsSkillAvailable(SkillName))
	{
		if (SkillPoints >= Skills[SkillName].Cost)
		{
			SpendSkillPoints(Skills[SkillName].Cost);
			Skills[SkillName].Acquired = true;
		}
	}
}

void APlayerCharacter::StartDash()
{
	FString SkillName = TEXT("Dash");
	if (Skills.Contains(SkillName) && Skills[SkillName].Acquired)
	{
		Super::StartDash();
	}
}

void APlayerCharacter::StartHeavyAttack()
{
	FString SkillName = TEXT("Heavy Attack");
	if (Skills.Contains(SkillName) && Skills[SkillName].Acquired)
	{
		Super::StartHeavyAttack();
	}
}

void APlayerCharacter::SaveGame()
{
	FSaveData SaveData;
	SaveData.PlayerPosition = GetActorTransform();
	SaveData.CurrentHealth = HealthComponent->GetCurrentHealth();
	SaveData.MaxHealth = HealthComponent->GetMaxHealth();
	SaveData.BaseDamage = BaseDamage;
	SaveData.CurrentExp = CurrentExp;
	SaveData.ExpToNextLevel = ExpToNextLevel;
	SaveData.PlayerLevel = PlayerLevel;
	SaveData.SkillPoints = SkillPoints;
	SaveData.Skills = Skills;
	SaveData.ItemsHeld = ItemsHeld;

	APlatformer3DPlayerController* PlayerController = Cast<APlatformer3DPlayerController>(Controller);
	if (PlayerController->SaveGame(SaveData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Successful"));
	}
}

void APlayerCharacter::LoadGame()
{
	FSaveData SaveData;

	APlatformer3DPlayerController* PlayerController = Cast<APlatformer3DPlayerController>(Controller);
	if (PlayerController->LoadGame(SaveData))
	{
		SetActorTransform(SaveData.PlayerPosition);
		HealthComponent->SetCurrentHealth(SaveData.CurrentHealth);
		HealthComponent->SetMaxHealth(SaveData.MaxHealth);
		BaseDamage = SaveData.BaseDamage;
		SaveData.CurrentExp = SaveData.CurrentExp;
		ExpToNextLevel = SaveData.ExpToNextLevel;
		PlayerLevel = SaveData.PlayerLevel;
		SkillPoints = SaveData.SkillPoints;
		Skills = SaveData.Skills;
		ItemsHeld = SaveData.ItemsHeld;
		UE_LOG(LogTemp, Warning, TEXT("Load Successful"));
	}
}

void APlayerCharacter::AddPickUpItem(EItem_Types ItemType, int Amount)
{
	if (ItemType != EItem_Types::IT_None && Amount > 0)
	{
		/*** Add check for amount >= 0 ***/
		if (ItemsHeld.Contains(ItemType))
		{
			ItemsHeld[ItemType] = ItemsHeld[ItemType] + Amount;
		}
		else
		{
			ItemsHeld.Add(ItemType, Amount);
		}
	}
}

int APlayerCharacter::GetPickUpItemNum(EItem_Types ItemType) const
{
	if (ItemsHeld.Contains(ItemType))
	{
		return ItemsHeld[ItemType];
	}

	return 0;
}

void APlayerCharacter::UseHealthPotion()
{
	if (ItemsHeld.Contains(EItem_Types::IT_HealthRecovery) && ItemsHeld[EItem_Types::IT_HealthRecovery] > 0)
	{
		if (HealthComponent->GetCurrentHealth() < HealthComponent->GetMaxHealth())
		{
			HealthComponent->RecoverHealth(HealthPotionRecovery);
			ItemsHeld[EItem_Types::IT_HealthRecovery] = ItemsHeld[EItem_Types::IT_HealthRecovery] - 1;
		}
	}
}