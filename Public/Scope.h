// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scope.generated.h"

UCLASS()
class VERSION_API AScope : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScope();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	class UStaticMesh* GetStaticMesh() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* mSceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneCaptureComponent2D* mSceneCapture;
	
};
