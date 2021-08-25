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
	//ĳ���͸� �̵��� �̵��ϴ� ������ ���ϵ��� ȸ���� ������ ����.
	//������ bUseControllerDesiredRotation�̳� bUseControllerRotationYaw���� ���ؼ�
	//false�� �ٲ���ϴ� ������ ���ؼ� ���� �ʿ�.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//540, 600 ���� ������ �������ִ� ������ ���� �ʿ�.
	//���� 0.0f�Ͻ� ĳ���Ͱ� ȸ������ �ʴ´�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	
	bUseControllerRotationYaw = false;

	mSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("mSpringArm"));
	mSpringArm->SetupAttachment(RootComponent);
	mSpringArm->TargetArmLength = 300.0f;
	mSpringArm->bUsePawnControlRotation = true; //

	//��� �޴� ���� ������Ʈ�� ȸ�� ��Ҹ� ���������� ����
	//���� Yaw ���� false�� �ٲٸ� ī�޶�(ĳ���� ������) ȸ������ �ʴ´�.
	mSpringArm->bInheritPitch = true;
	mSpringArm->bInheritRoll = false;
	mSpringArm->bInheritYaw = true;


	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("mCamera"));
	mCamera->SetupAttachment(mSpringArm, USpringArmComponent::SocketName);
	//������ �Ͽ� ���� ��� ��ǥ�� ī�޶� ��ġ ����.
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

	//�Է¿� ���� ó���� �Լ����� ���ε��Ѵ�.
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	//���콺 X, Y�࿡ ���� �������� controller�� �ѱ�.
	PlayerInputComponent->BindAxis("MoveMouseX", this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MoveMouseY", this, &AMyCharacter::AddControllerPitchInput);

}

void AMyCharacter::MoveForward(float value)
{
	/*
		�Է°��� ���ؼ� �Ʒ����� �ڵ带 �̿��ϴ� ������ ������ �ʿ�.
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