// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include <Engine/Classes/GameFramework/CharacterMovementComponent.h>
#include <Engine/Classes/GameFramework/Controller.h>
#include <Engine/Classes/GameFramework/SpringArmComponent.h>
#include <Engine/Classes/Camera/CameraComponent.h>
#include <Engine/classes/Components/CapsuleComponent.h>

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	//캐릭터를 이동시 이동하는 방향을 향하도록 회전할 것인지 여부.
	//설정시 bUseControllerDesiredRotation이나 bUseControllerRotationYaw값에 대해서
	//false로 바꿔야하는 이유에 대해서 조자 필요.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//540, 600 등의 각도를 지정해주는 이유는 조사 필요.
	//값이 0.0f일시 캐릭터가 회전하지 않는다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	
	bUseControllerRotationYaw = false;

	mSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("mSpringArm"));
	mSpringArm->SetupAttachment(RootComponent);
	mSpringArm->TargetArmLength = 300.0f;
	mSpringArm->bUsePawnControlRotation = true; //

	//상속 받는 하위 컴포넌트의 회전 요소를 조정할지의 여부
	//예로 Yaw 값을 false로 바꾸면 카메라가(캐릭터 주위를) 회전하지 않는다.
	mSpringArm->bInheritPitch = true;
	mSpringArm->bInheritRoll = false;
	mSpringArm->bInheritYaw = true;


	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("mCamera"));
	mCamera->SetupAttachment(mSpringArm, USpringArmComponent::SocketName);
	//스프링 암에 대해 상대 좌표로 카메라 위치 설정.
	mCamera->SetRelativeLocation(FVector(-50.0f, 0.0f, 250.0f));
	mCamera->SetRelativeRotation(FRotator(-35.0f, 0.0f, 0.0f));
	mCamera->bUsePawnControlRotation = false;		//

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using fps"));
	}

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//입력에 대해 처리할 함수들을 바인드한다.
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	//마우스 X, Y축에 관한 움직임은 controller에 넘김.
	PlayerInputComponent->BindAxis("MoveMouseX", this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MoveMouseY", this, &AMyCharacter::AddControllerPitchInput);

}

void AMyCharacter::MoveForward(float value)
{
	/*
		입력값에 대해서 아래같은 코드를 이용하는 이유를 조사할 필요.
	*/
	if ((Controller != NULL) && (value != 0.0f))
	{
		FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, value);
	}
}

void AMyCharacter::MoveRight(float value)
{
	if ((Controller != NULL) && (value != 0.0f))
	{
		FVector direction = FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, value);
	}
}

void AMyCharacter::MoveMouseX(float value)
{
	value = FMath::Clamp(value, -1.0f, 1.0f);
	AddControllerYawInput(0);

}

void AMyCharacter::MoveMouseY(float value)
{
	AddControllerPitchInput(0);

}