// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Platformer3DCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// APlatformer3DCharacter

APlatformer3DCharacter::APlatformer3DCharacter()
{
	CharacterMovementComponent = GetCharacterMovement();

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 1.0f;

	// Create Health Component
	HealthComponent = CreateDefaultSubobject<UHealthActorComponent>(TEXT("HEALTH"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void APlatformer3DCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterController = GetController();
}

void APlatformer3DCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CharacterMovementComponent->IsFalling() && !IsDashing && CharacterMovementComponent->Velocity.Z < 0.0f)
	{
		CharacterMovementComponent->GravityScale = FallSpeedRatio;
	}

	if (TargetLocked || NearestTarget != nullptr)
	{
		LookAt(NearestTarget->GetActorLocation(), DeltaTime);
	}

	// Ugly and hacky way to simulate a cooldown timer
	if (AttackCooldown > 0.f)
	{
		AttackCooldown -= DeltaTime;
	}
}

void APlatformer3DCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlatformer3DCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlatformer3DCharacter::LookAt(FVector Location, float Rate) {
	FRotator ControlRotation = CharacterController->GetControlRotation();

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	ControlRotation.Yaw = FMath::RInterpTo(ControlRotation, TargetRotation, Rate, 0.0f).Yaw;
	CharacterController->SetControlRotation(ControlRotation);
}

void APlatformer3DCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlatformer3DCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APlatformer3DCharacter::DisableMoveInput()
{
	CharacterController->SetIgnoreMoveInput(true);
}

void APlatformer3DCharacter::EnableMoveInput()
{
	CharacterController->ResetIgnoreMoveInput();
}

void APlatformer3DCharacter::StartJump()
{
	if (IsDashing || RollDodgeAnimation != 0 || SaveAttack)
		return;

	// Stop any montage playing, it should already be on an overridable state
	StopAttackMontage();

	Jump();

	if (CharacterMovementComponent->IsFalling() && JumpCount < MaxJumpCount)
	{
		FVector JumpSpeed = FVector(0.0f, 0.0f, CharacterMovementComponent->JumpZVelocity);

		CharacterMovementComponent->GravityScale = 1;
		LaunchCharacter(JumpSpeed, false, true);
		JumpCount++;
	}

	if (!DashedOnAir)
	{
		ResetDash();
	}
}

void APlatformer3DCharacter::EndJump()
{
	if (IsDashing || RollDodgeAnimation != 0 || SaveAttack)
		return;

	StopJumping();

	if (CharacterMovementComponent->IsFalling() && !IsDashing)
	{
		CharacterMovementComponent->GravityScale = FastFallSpeedRatio;
	}
}

void APlatformer3DCharacter::StartDash()
{
	if (CanDash && !DashedOnAir && !SaveAttack)
	{
		// Stop any montage playing, it should already be on an overridable state
		StopAttackMontage();

		// This blocks any imput, preventing any action, use other method if wanted to allow some actions while dashing
		DisableMoveInput();

		HorizontalSpeed = CharacterMovementComponent->Velocity;
		HorizontalSpeed.Z = 0.0f;
		FVector DashDirection = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f);

		CharacterMovementComponent->BrakingFrictionFactor = 0.f;
		CharacterMovementComponent->GravityScale = 0.f;
		CharacterMovementComponent->Velocity = DashDirection.GetSafeNormal() * DashSpeed;
		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlatformer3DCharacter::EndDash, DashDuration, false);

		IsDashing = true;
		CanDash = false;
		if (CharacterMovementComponent->IsFalling())
			DashedOnAir = true;
	}
}

void APlatformer3DCharacter::EndDash()
{
	EnableMoveInput();

	CharacterMovementComponent->StopMovementImmediately();
	CharacterMovementComponent->Velocity = HorizontalSpeed;
	CharacterMovementComponent->BrakingFrictionFactor = 2.f;
	CharacterMovementComponent->GravityScale = 1.f;
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlatformer3DCharacter::ResetDash, DashCooldownTime, false);

	IsDashing = false;
}

void APlatformer3DCharacter::ResetDash()
{
	CanDash = true;
}

bool APlatformer3DCharacter::GetIsDashing()
{
	return IsDashing;
}

void APlatformer3DCharacter::Landed(const FHitResult& Hit)
{
	ACharacter::Landed(Hit);

	CharacterMovementComponent->GravityScale = 1;
	JumpCount = 0;
	DashedOnAir = false;
}

void APlatformer3DCharacter::StartRun()
{
	CharacterMovementComponent->MaxWalkSpeed = RunSpeed;
}

void APlatformer3DCharacter::EndRun()
{
	CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
}

void APlatformer3DCharacter::RollDodge()
{
	// If RollDodgeAction can�t be deduced, and Animation isn't already playing
	if (RollDodgeAction < 2 && RollDodgeAnimation == 0)
	{
		RollDodgeAction++;
		GetWorldTimerManager().ClearTimer(RollTimerHandle);
		GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APlatformer3DCharacter::ExecuteRollDodge, 0.17f, false);
	}
}

void APlatformer3DCharacter::ExecuteRollDodge()
{
	//If character is doing a blocking animation, they can�t roll/dodge (possible to make this check on RollDodge() to prevent creation of unnecessary timers).
	if (CharacterMovementComponent->IsFalling() || SaveAttack)
	{
		ResetRollDodgeAnimation();
		return;
	}

	if (RollDodgeAction >= 2)
	{
		RollDodgeAction = 2;
	}

	// Stop any montage playing, it should already be on an overridable state
	StopAttackMontage();

	// Little hack to set animation index to 0 or 4, to offset wether action is a roll or a dodge
	// Followed by a galaxy brain hack, if there is no direction pressed make animation index 0 so no animation will be played
	RollDodgeAnimation = ((RollDodgeAction - 1) * 4);

	//if (!TargetLocked)
	{
		RollDodgeAnimation += 1;
	}
	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	/*else
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
	}*/

	float ResetTimer = RollDodgeAnimation <= 4 ? 0.7 : 0.3;
	GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APlatformer3DCharacter::ResetRollDodgeAnimation, ResetTimer, false);
}

void APlatformer3DCharacter::ResetRollDodgeAnimation()
{
	RollDodgeAction = 0;
	RollDodgeAnimation = 0;
}

int APlatformer3DCharacter::GetRollDodgeAnimation()
{
	return RollDodgeAnimation;
}

void APlatformer3DCharacter::LockOnTarget(AActor* Target)
{
	NearestTarget = Target;
	TargetLocked = true;
}

void APlatformer3DCharacter::LockOffTarget()
{
	TargetLocked = false;
	NearestTarget = nullptr;
}

void APlatformer3DCharacter::StartAttack()
{
	// Ugly and hacky way to simulate a cooldown timer, check Tick() function for update
	if (!SaveAttack && AttackCooldown <= 0.f)
	{			
		IsAttacking = true;
		SaveAttack = true;

		DisableMoveInput();

		/***** Missing set launch force and jump force for each montage *****/
		LaunchForce = 500.f;

		ComboCount = (ComboCount + 1) % AttackMontages.Num();

		AttackCooldown = 0.8;
	
		// There may be a better place to make these checks and a way to handle if any of them fails
		if (AttackMontages.Num() > 0 && ComboCount < AttackMontages.Num() && AttackMontages[ComboCount])
		{
			PlayAnimMontage(AttackMontages[ComboCount]);
		}
	}
}

void APlatformer3DCharacter::SaveComboAttack()
{
	SaveAttack = false;
}

void APlatformer3DCharacter::ResetCombo()
{
	IsAttacking = false;
	SaveAttack = false;
	ComboCount = 0;
	JumpForce = 0.f;
	LaunchForce = 0.f;
	EnableMoveInput();
}

void APlatformer3DCharacter::ApplyAttackLaunch()
{
	FVector AttackDirection = GetActorForwardVector() * LaunchForce + GetActorUpVector() * JumpForce;

	CharacterMovementComponent->GroundFriction = 0.f;
	CharacterMovementComponent->Velocity = AttackDirection;
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlatformer3DCharacter::EndAttackLaunch, 0.25f, false);
}

void APlatformer3DCharacter::EndAttackLaunch()
{
	CharacterMovementComponent->GroundFriction = 8.f;
}

void APlatformer3DCharacter::StopAttackMontage()
{
	StopAnimMontage(AttackMontages[ComboCount]);
	ResetCombo();
}

void APlatformer3DCharacter::RegisterAttackHitbox(UShapeComponent* Hitbox)
{
	AttackHitbox = Hitbox;
	// Hitbox is usually attached to a weapon, which is probably a Mesh, check if it exists and set collision presets
	UStaticMeshComponent* WeaponMesh = Cast<UStaticMeshComponent>(AttackHitbox->GetAttachParent());
	if (WeaponMesh)
	{
		WeaponMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	}

	// Set collision presets
	AttackHitbox->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	AttackHitbox->SetCollisionProfileName(TEXT("Attacks"));
	AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlatformer3DCharacter::OnAttackOverlap);
}

void APlatformer3DCharacter::OnAttackOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, CharacterController, this, NULL);
	}
}

void APlatformer3DCharacter::EnableAttackHitBox()
{
	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void APlatformer3DCharacter::DisableAttackHitBox()
{
	if (AttackHitbox)
	{
		AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

float APlatformer3DCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (EventInstigator != CharacterController)
	{
		HealthComponent->DecreaseHealth(Damage);
		ReactToDamage();
	}

	return Damage;
}

void APlatformer3DCharacter::ReactToDamage()
{
	if (HealthComponent->IsAlive())
	{
		if (DamageMontage)
		{
			PlayAnimMontage(DamageMontage);
		}
	}
	else
	{
		CharacterMovementComponent->DisableMovement();
		// Lock off target and other stuff people do when they die
		if (DeathMontage)
		{
			PlayAnimMontage(DeathMontage);
		}
		// Game Over or something
	}
}