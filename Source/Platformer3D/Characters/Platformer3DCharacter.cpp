// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Platformer3DCharacter.h"

// Engine libraries
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

	// Create Attack System Component
	AttackSystem = CreateDefaultSubobject<UAttackSystemComponent>(TEXT("Attack System"));

	// Create Health Component
	HealthComponent = CreateDefaultSubobject<UHealthActorComponent>(TEXT("Health"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void APlatformer3DCharacter::BeginPlay()
{
	Super::BeginPlay();

	Controller = GetController();
}

void APlatformer3DCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->IsFalling() && !IsDashing && GetCharacterMovement()->Velocity.Z < 0.0f && !AttackSystem->IsAttackAnimation() && !IsFlinching)
	{
		GetCharacterMovement()->GravityScale = FallSpeedRatio;
	}

	/***** Should camera be able to keep track of target even when performing blocking actions??? *****/
	/***** In any case, player shouldn't change rotation *****/
	if (TargetLocked && NearestTarget && !AttackSystem->IsAttacking() && !IsDashing && RollDodgeAnimation == 0)
	{
		LookAt(NearestTarget->GetActorLocation(), DeltaTime);
	}
}

void APlatformer3DCharacter::TurnAtRate(float Rate)
{
	if (!TargetLocked)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APlatformer3DCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlatformer3DCharacter::LookAt(FVector Location, float Rate) {
	FRotator ControlRotation = Controller->GetControlRotation();

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	ControlRotation.Yaw = FMath::RInterpTo(ControlRotation, TargetRotation, Rate, 0.0f).Yaw;
	Controller->SetControlRotation(ControlRotation);
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

void APlatformer3DCharacter::ResetMoveState()
{
	AttackSystem->CancelAttack();
	DisableAttackHitBox();
	SetCrouchState(false);

	EnableMoveInput();
}

void APlatformer3DCharacter::SetCrouchState(bool DoCrouch)
{
	/***** Can crouch property has to be set via blueprint since there is no method to change it via ACharacterMovementComponent once created and ACharacter creates it's own component *****/
	if (DoCrouch)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void APlatformer3DCharacter::ToggleCrouchState()
{
	/***** Can crouch property has to be set via blueprint since there is no method to change it via ACharacterMovementComponent once created and ACharacter creates it's own component *****/
	if (GetMovementComponent()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

bool APlatformer3DCharacter::GetIsCrouching()
{
	return GetMovementComponent()->IsCrouching();
}

void APlatformer3DCharacter::DisableMoveInput()
{
	Controller->SetIgnoreMoveInput(true);
}

void APlatformer3DCharacter::EnableMoveInput()
{
	Controller->ResetIgnoreMoveInput();
}

void APlatformer3DCharacter::DisableLookupInput()
{
	Controller->SetIgnoreLookInput(true);
}

void APlatformer3DCharacter::EnableLookupMoveInput()
{
	Controller->ResetIgnoreLookInput();
}

void APlatformer3DCharacter::StartJump()
{
	if (IsDashing || RollDodgeAnimation != 0 || AttackSystem->IsAttackAnimation())
		return;

	ResetMoveState();
	
	Jump();

	if (GetCharacterMovement()->IsFalling() && JumpCount < MaxJumpCount)
	{
		FVector JumpSpeed = FVector(0.f, 0.f, GetCharacterMovement()->JumpZVelocity);

		GetCharacterMovement()->GravityScale = 1.f;
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
	if (IsDashing || RollDodgeAnimation != 0 || AttackSystem->IsAttackAnimation())
		return;

	StopJumping();

	if (GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->GravityScale = FastFallSpeedRatio;
	}
}

void APlatformer3DCharacter::StartDash()
{
	if (CanDash && !DashedOnAir && !AttackSystem->IsAttackAnimation() && RollDodgeAnimation == 0)
	{
		// Stop any montage playing, it should already be on an overridable state
		ResetMoveState();

		// This blocks any imput, preventing any action, use other method if wanted to allow some actions while dashing
		DisableMoveInput();

		HorizontalSpeed = GetCharacterMovement()->Velocity;
		HorizontalSpeed.Z = 0.0f;
		FVector DashDirection = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f);

		GetCharacterMovement()->BrakingFrictionFactor = 0.f;
		GetCharacterMovement()->GravityScale = 0.f;
		GetCharacterMovement()->Velocity = DashDirection.GetSafeNormal() * DashSpeed;
		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlatformer3DCharacter::EndDash, DashDuration, false);

		IsDashing = true;
		CanDash = false;
		if (GetCharacterMovement()->IsFalling())
			DashedOnAir = true;
	}
}

void APlatformer3DCharacter::EndDash()
{
	EnableMoveInput();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = HorizontalSpeed;
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
	GetCharacterMovement()->GravityScale = 1.f;
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
	ResetJumpState();
	GetCharacterMovement()->GravityScale = 1;
	JumpCount = 0;
	DashedOnAir = false;
}

void APlatformer3DCharacter::StartRun()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APlatformer3DCharacter::EndRun()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlatformer3DCharacter::RollDodge()
{
	// If RollDodgeAction can´t be deduced, and Animation isn't already playing
	if (RollDodgeAction < 2 && RollDodgeAnimation == 0)
	{
		RollDodgeAction++;
		GetWorldTimerManager().ClearTimer(RollTimerHandle);
		GetWorldTimerManager().SetTimer(RollTimerHandle, this, &APlatformer3DCharacter::ExecuteRollDodge, 0.17f, false);
	}
}

void APlatformer3DCharacter::ExecuteRollDodge()
{
	//If character is doing a blocking animation, they can´t roll/dodge (possible to make this check on RollDodge() to prevent creation of unnecessary timers).
	if (GetCharacterMovement()->IsFalling() || AttackSystem->IsAttackAnimation())
	{
		ResetRollDodgeAnimation();
		return;
	}

	if (RollDodgeAction >= 2)
	{
		RollDodgeAction = 2;
	}

	ResetMoveState();

	// Little hack to set animation index to 0 or 4, to offset wether action is a roll or a dodge
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
	/***** Since End Attack is callde via Anim Montage signal, try to avoid changing parameters that need to be reset on End Attack *****/
	/***** At the very least, i should do similar steps to End Attack whenever a Cancel Attack is needed, maybe even refactor both functions into one *****/
	if (AttackSystem->CanAttack())
	{
		DisableMoveInput();
		if (GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->GravityScale = 0.f;
			AttackSystem->AerialAttack();
		}
		else
		{
			GetCharacterMovement()->StopMovementImmediately();
			AttackSystem->NormalAttack();
		}
	}
}

void APlatformer3DCharacter::StartHeavyAttack()
{
	if (AttackSystem->CanAttack() && !GetCharacterMovement()->IsFalling())
	{
		DisableMoveInput();
		GetCharacterMovement()->StopMovementImmediately();
		AttackSystem->HeavyAttack();
	}
}

void APlatformer3DCharacter::EndAttack()
{
	AttackSystem->ResetCombo();
	GetCharacterMovement()->GravityScale = 1.f;
	EnableMoveInput();
}

void APlatformer3DCharacter::SaveCombo()
{
	AttackSystem->SaveComboAttack();
}

void APlatformer3DCharacter::ApplyAttackLaunch()
{
	FAttack CurrentAttack = AttackSystem->GetCurrentAttack();
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->GravityScale = CurrentAttack.JumpForce > 0.f ? 0.f : 1.f;
	GetCharacterMovement()->Velocity = GetActorForwardVector() * CurrentAttack.AdvanceForce;
	GetCharacterMovement()->Launch(GetActorUpVector() * CurrentAttack.JumpForce);
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlatformer3DCharacter::EndAttackLaunch, 1.f, false);
}

void APlatformer3DCharacter::EndAttackLaunch()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->GroundFriction = 8.f;
}

void APlatformer3DCharacter::StartRangedAttack()
{
	if (AttackSystem->CanAttack())
	{
		AttackSystem->AimRangedAttack();
	}
}

void APlatformer3DCharacter::EndRangedAttack()
{
	if (AttackSystem->IsAiming())
	{
		AttackSystem->FireRangedAttack();
	}
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

void APlatformer3DCharacter::OnAttackOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor->GetClass()->ImplementsInterface(UDamagableObject_Interface::StaticClass()))
	{
		IDamagableObject_Interface* DamagableObject = Cast<IDamagableObject_Interface>(OtherActor);
		if (DamagableObject && DamagableObject->GetIsAlive())
		{
			DoDamage(OtherActor);

			if (GetCharacterMovement()->IsFalling())
			{
				if (GetCharacterMovement()->Velocity.Z < 0.0f)
				{
					GetCharacterMovement()->StopMovementImmediately();
				}
				GetCharacterMovement()->GravityScale = 0.f;
			}
			DamagableObject->ReactToDamage(AttackSystem->GetCurrentAttack().LaunchForce);
		}
	}
}

void APlatformer3DCharacter::ReactToDamage(float AttackForce)
{
	IDamagableObject_Interface::ReactToDamage(AttackForce);

	if (HealthComponent->IsAlive())
	{

		AttackSystem->CancelAttack();
		DisableAttackHitBox();

		if (DamageMontage)
		{
			GetCharacterMovement()->StopMovementImmediately();
			DisableMoveInput();
			GetCharacterMovement()->GravityScale = 0.f;
			IsFlinching = true;
			PlayAnimMontage(DamageMontage);
			GetCharacterMovement()->Launch(GetActorUpVector() * AttackForce);
		}

		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
		GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &APlatformer3DCharacter::EndReactToDamage, 1.f, false);
	}
}

void APlatformer3DCharacter::EndReactToDamage()
{
	GetCharacterMovement()->StopMovementImmediately();
	EnableMoveInput();
	IsFlinching = false;
	GetCharacterMovement()->GravityScale = 1.f;
}

void APlatformer3DCharacter::DoDamage(AActor* Target)
{
	float Damage = UKismetMathLibrary::RandomFloatInRange(BaseDamage, AttackSystem->GetAttackDamage());
	UGameplayStatics::ApplyDamage(Target, Damage, Controller, this, NULL);
}

float APlatformer3DCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (EventInstigator != Controller)
	{
		HealthComponent->DecreaseHealth(Damage);
		if (GetCurrentHealth() <= 0)
		{
			GetCharacterMovement()->DisableMovement();
			// Lock off target and other stuff people do when they die
			if (DeathMontage)
			{
				LockOffTarget();
				DeathMontage->bEnableAutoBlendOut = false;
				PlayAnimMontage(DeathMontage);
			}
			// Game Over or something
		}
	}

	return Damage;
}
float APlatformer3DCharacter::GetCurrentHealth() const
{
	if (HealthComponent)
	{
		return HealthComponent->GetCurrentHealth();
	}

	return 0.f;
}

float APlatformer3DCharacter::GetCurrentHealthPercent() const
{
	if (HealthComponent && HealthComponent->GetMaxHealth() > 0)
	{
		return HealthComponent->GetCurrentHealth() / HealthComponent->GetMaxHealth();
	}

	return 0.f;
}

bool APlatformer3DCharacter::GetIsAlive() const
{
	if (HealthComponent)
	{
		return HealthComponent->IsAlive();
	}

	return false;
}