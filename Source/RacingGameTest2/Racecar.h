// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Templates/Casts.h"
// #include "Blueprint/UserWidget.h"
#include "Racecar.generated.h"

UCLASS()
class RACINGGAMETEST2_API ARacecar : public APawn
{
	GENERATED_BODY()

	enum State{air,ground};

public:
	// Sets default values for this pawn's properties
	ARacecar();

	UPROPERTY(EditAnywhere)	
	float MAX_SPEED = 50;

	UPROPERTY(EditAnywhere)	
	float ACC_SPEED = 0.5;

	UPROPERTY(EditAnywhere)	
	float SLIDE_ROT_SPEED = 1.0;

	UPROPERTY(EditAnywhere)	
	float SLIDE_DEC_SPEED = 0.5;

	UPROPERTY(EditAnywhere)
	float SLIDE_MIN_SPEED = 25.0;

	UPROPERTY(EditAnywhere)	
	float GRAVITY = 0.5;

	UPROPERTY(EditAnywhere)
	float RESPAWNPLANE = -1000;

	UPROPERTY(EditAnywhere)
	float rotationInterpolationSpeed = 0.4;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void Move_XAxis(float value);
	void Move_YAxis(float value);

	void StartAccMove();
	void StopAccMove();

	void StartSlide();
	void StopSlide();

	float ForwardVelocity;
	bool accelerating, sliding;

	FVector airVelocity;
	FVector startPos;

	float turning;

	float slideVelocity;

	State cState;

};
