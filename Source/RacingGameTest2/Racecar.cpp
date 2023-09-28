// Fill out your copyright notice in the Description page of Project Settings.


#include "Racecar.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Math/Color.h"
#include "Math/Rotator.h"
#include "Math/MathFwd.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Templates/Casts.h"
#include <cmath>

// Sets default values
ARacecar::ARacecar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	accelerating = sliding = false;
	turning = 0;
	ForwardVelocity = 0;
	cState = ground;
}	

// Called when the game starts or when spawned
void ARacecar::BeginPlay()
{
	Super::BeginPlay();
	startPos = GetActorLocation();
	// comp = FindComponentByClass<UStaticMeshComponent>();

}

// Called every frame
void ARacecar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	UWorld* world = GetWorld();
	FHitResult outhit;

	bool noBlockingHit = true;
	
	bool isHit = world->LineTraceSingleByChannel(outhit, 
		GetActorLocation()+FVector(0,0,100),
		 GetActorLocation()+FVector(0,0,-100),
		  ECC_Visibility);
	
	switch(cState){
		case ground:	
		
			// CONTROL
			if(accelerating && ForwardVelocity < MAX_SPEED){
				ForwardVelocity += ACC_SPEED;
			}
			else if(ForwardVelocity >= ACC_SPEED) {
				ForwardVelocity -= ACC_SPEED;
			}
			AddActorLocalOffset(FVector(0,-ForwardVelocity*60*DeltaTime,0));
			AddActorLocalRotation(UE::Math::TRotator<double>(0,(turning+slideVelocity)*DeltaTime*60*ForwardVelocity/MAX_SPEED,0));

			if(sliding){
				slideVelocity = turning*SLIDE_ROT_SPEED;
				slideVelocity = FMath::Clamp(slideVelocity,-SLIDE_ROT_SPEED,SLIDE_ROT_SPEED);
				if(ForwardVelocity > SLIDE_MIN_SPEED){
					ForwardVelocity -= SLIDE_DEC_SPEED;
				}
			}
			else
			{
				slideVelocity = 0;//std::lerp(slideVelocity,0.0f,0.025f);
			}


			// GROUND COLLISION
			if(isHit && outhit.bBlockingHit){
				// UE_LOG(LogTemp,Log,TEXT("HITTING SOMETHING: %f"),outhit.ImpactPoint.Z);
				// UE_LOG(LogTemp,Log,TEXT("NORMAL: %f,%f,%f"),outhit.ImpactNormal.X,outhit.ImpactNormal.Y,outhit.ImpactNormal.Z);

				FVector vec1 = FVector::CrossProduct(outhit.ImpactNormal, -GetActorRightVector()) ;
				FVector vec0 = FVector::CrossProduct(outhit.ImpactNormal, vec1) ;
				FVector vec2 = outhit.ImpactNormal;
				FVector origin = GetActorLocation()-FVector(0,0,GetActorLocation().Z)+FVector(0,0,outhit.ImpactPoint.Z);

				FMatrix mat = FMatrix::Identity;
				mat.SetAxes(&vec1,&vec0,&vec2,&origin);
				SetActorRotation(FQuat::Slerp(GetActorQuat(),FQuat::MakeFromRotator(mat.Rotator()),rotationInterpolationSpeed));

				// DrawDebugLine(GetWorld(),GetActorLocation(),GetActorLocation()+vec1*100,FColor(0,255,0),false,10,1,1.0f);

				// set actor y position
				SetActorLocation(GetActorLocation()-FVector(0,0,GetActorLocation().Z)+FVector(0,0,outhit.ImpactPoint.Z));
			}
			else {
				cState = air;
				airVelocity = ForwardVelocity * (-GetActorRightVector());
			}
			break;
		case air:

			AddActorWorldOffset(airVelocity);

			airVelocity.Z -= GRAVITY;

			FVector vec1 = -airVelocity.GetSafeNormal();
			FVector vec0 = FVector::CrossProduct(vec1, GetActorUpVector());
			FVector vec2 = FVector::CrossProduct(vec0, vec1);
			FVector origin = GetActorLocation();

			FVector mov = FVector::CrossProduct(airVelocity.GetSafeNormal(),GetActorUpVector());

			FMatrix mat = FMatrix::Identity;
			mat.SetAxes(&vec0,&vec1,&vec2,&origin);

			SetActorRotation(FQuat::Slerp(GetActorQuat(),FQuat::MakeFromRotator(mat.Rotator()),0.2));

			if(GetActorLocation().Z < RESPAWNPLANE){
				SetActorLocation(startPos);
				airVelocity = FVector::ZeroVector;
				ForwardVelocity = 0;
				SetActorRotation(FRotator::ZeroRotator);
			}


			if(isHit && outhit.bBlockingHit){
				cState = ground;
			}

			break;
	}


	// UE_LOG(LogTemp, Log, TEXT("VELOCITY: %f | ACC: %d"),ForwardVelocity,accelerating);
}

// Called to bind functionality to input
void ARacecar::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Respond when Accelerate is pressed or released
	InputComponent->BindAction("Accel", IE_Pressed, this, &ARacecar::StartAccMove);
	InputComponent->BindAction("Accel",IE_Released,this, &ARacecar::StopAccMove);

	InputComponent->BindAction("Slide",IE_Pressed,this, &ARacecar::StartSlide);
	InputComponent->BindAction("Slide",IE_Released,this, &ARacecar::StopSlide);

	// // Respond when movement axes change
	InputComponent->BindAxis("RightAxis", this, &ARacecar::Move_XAxis);
	InputComponent->BindAxis("UpAxis", this, &ARacecar::Move_YAxis);

}

void ARacecar::StartAccMove(){
	accelerating = true;
	
}

void ARacecar::StopAccMove(){
	accelerating = false;
}

void ARacecar::StartSlide(){
	sliding = true;
}

void ARacecar::StopSlide(){
	sliding = false;
}

void ARacecar::Move_XAxis(float value){
	turning = value;
}

void ARacecar::Move_YAxis(float value){
	// stub
}