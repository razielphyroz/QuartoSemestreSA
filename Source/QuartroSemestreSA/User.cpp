#include "User.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "ManagerClass.h"

AUser::AUser()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->SetupAttachment(GetSprite());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	Camera->OrthoWidth = 512.0f;
	Camera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	GetCharacterMovement()->MaxWalkSpeed = 250;

	Life = 100;
	InitialLife = Life;
}

void AUser::BeginPlay()
{ 
	Super::BeginPlay(); 
	UWorld* World = GetWorld();
	if (World != nullptr) {

		if (MyScreen != NULL) {
			APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);
			if (Controller != nullptr) {
				UserWidget = UWidgetBlueprintLibrary::Create(World, MyScreen, Controller);
				if (UserWidget != nullptr) {
					UserWidget->AddToViewport();
				}
			}
		}

		TArray<AActor*> ActorsArray;
		UGameplayStatics::GetAllActorsOfClass(World, AManagerClass::StaticClass(), ActorsArray);

		if (ActorsArray.Num() >= 1) {
			Manager = Cast<AManagerClass>(ActorsArray[0]);
		}
	}
}

void AUser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();
}

void AUser::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveLeftRight", this, &AUser::MoveLeftRight);
	PlayerInputComponent->BindAxis("MoveUpDown", this, &AUser::MoveUpDown);
}

int AUser::GetLife()
{
	return Life;
}

void AUser::SetLife(int Value)
{
	if (Value >= InitialLife) {
		Life = InitialLife;
	} else if (Value <= 0) {
		Life = 0;
	} else {
		Life = Value;
	}
}

int AUser::GetAmmoEnergy()
{
	return AmmoEnergy;
}

void AUser::SetAmmoEnergy(int Value)
{
	AmmoEnergy = Value;
}

int AUser::GetAmmoLaser()
{
	return AmmoLaser;
}

void AUser::SetAmmoLaser(int Value)
{
	AmmoLaser = Value;
}

void AUser::MoveUpDown(float Value)
{ VelY = Value; }

void AUser::MoveLeftRight(float Value)
{ VelX = Value; }

void AUser::Move()
{
	FlipbookAtual = GetSprite()->GetFlipbook();

	if (VelX == 0 && VelY == 0) {
		if (FlipbookAtual == FlipbooksArray[4]) {
			GetSprite()->SetFlipbook(FlipbooksArray[0]);
		} else if (FlipbookAtual == FlipbooksArray[5]) {
			GetSprite()->SetFlipbook(FlipbooksArray[1]);
		} else if (FlipbookAtual == FlipbooksArray[6]) {
			GetSprite()->SetFlipbook(FlipbooksArray[2]);
		} else if (FlipbookAtual == FlipbooksArray[7]) {
			GetSprite()->SetFlipbook(FlipbooksArray[3]);
		}
	} else {
		if (FMath::Abs(VelX) >= FMath::Abs(VelY)) {
			if (VelX < 0 && FlipbookAtual != FlipbooksArray[6]) {
				GetSprite()->SetFlipbook(FlipbooksArray[6]);
			} else if (VelX > 0 && FlipbookAtual != FlipbooksArray[7]) {
				GetSprite()->SetFlipbook(FlipbooksArray[7]);
			}
		} else {
			if (VelY < 0 && FlipbookAtual != FlipbooksArray[4]) {
				GetSprite()->SetFlipbook(FlipbooksArray[4]);
			} else if (VelY > 0 && FlipbookAtual != FlipbooksArray[5]) {
				GetSprite()->SetFlipbook(FlipbooksArray[5]);
			}
		}
	}

	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), VelY);
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), VelX);
}

TSubclassOf<class AUserProjectile> AUser::ShotType()
{
	if (PosToAcessArray == 1) {
		AmmoLaser--;
	} else if (PosToAcessArray == 2) {
		AmmoEnergy--;
	}

	return ProjectilesArray[PosToAcessArray];
}

void AUser::CheckAmmo()
{
	if (PosToAcessArray == 1 && AmmoLaser <= 0 ||
		PosToAcessArray == 2 && AmmoEnergy <= 0) {
		PosToAcessArray = 0;
	}
}

void AUser::ChangeShot(int Value)
{
	if (ProjectilesArray.Num() > 0 && Value >= 0 && Value <= ProjectilesArray.Num()) {
		switch (Value) {
		case 1:
			AmmoEnergy = 0;
			break;
		case 2:
			AmmoLaser = 0;
			break;
		}
		PosToAcessArray = Value;
	}
}