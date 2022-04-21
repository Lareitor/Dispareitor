// Fill out your copyright notice in the Description page of Project Settings.


#include "DispareitorInstanciaAnimacion.h"
#include "DispareitorPersonaje.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDispareitorInstanciaAnimacion::NativeInitializeAnimation() {
    Super::NativeInitializeAnimation();

    DispareitorPersonaje = Cast<ADispareitorPersonaje>(TryGetPawnOwner());
}

void UDispareitorInstanciaAnimacion::NativeUpdateAnimation(float DeltaTime) {   
    Super::NativeUpdateAnimation(DeltaTime);

    if(DispareitorPersonaje == nullptr) {
        DispareitorPersonaje = Cast<ADispareitorPersonaje>(TryGetPawnOwner());
        if(DispareitorPersonaje == nullptr) {
          return; 
        }
    }

    FVector VelocidadTemporal = DispareitorPersonaje->GetVelocity();
    VelocidadTemporal.Z = 0.f;
    Velocidad = VelocidadTemporal.Size();

    bEstaEnElAire = DispareitorPersonaje->GetCharacterMovement()->IsFalling();

    bEstaAcelerando = DispareitorPersonaje->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

}