// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUpItem.h"
#include "Components/SphereComponent.h"
#include "Characters/PlayerCharacter.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Scene Root"));
	
	/*StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));*/
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();

	if (SphereComponent)
	{
		SphereComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		SphereComponent->SetCollisionProfileName(TEXT("PlayerTrigger"));
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpItem::OnPawnOverlap);
	}

	
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpItem::OnPawnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->AddPickUpItem(ItemType, Number);
		Destroy();
	}
}
