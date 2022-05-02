// Fill out your copyright notice in the Description page of Project Settings.


#include "DispareitorInstanciaAnimacion.h"
#include "DispareitorPersonaje.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

    bEnElAire = DispareitorPersonaje->GetCharacterMovement()->IsFalling();
    bAcelerando = DispareitorPersonaje->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
    bArmaEquipada = DispareitorPersonaje->EstaArmaEquipada();
    bAgachado = DispareitorPersonaje->bIsCrouched;
    bApuntando = DispareitorPersonaje->EstaApuntando();

    // GiroDesviacion para el Strafing 
    // Es una rotacion global, independiente de adonde mire el personaje. Si esta mirando al X global sera 0. Al rotar hacia la derecha se incrementa hasta 180. Luego pasa -180 y decrece hasta 0.
    FRotator RotacionApuntado = DispareitorPersonaje->GetBaseAimRotation();
    // MakeRotFromX toma un vector como direccion y calcula su rotacion
    FRotator RotacionMovimiento = UKismetMathLibrary::MakeRotFromX(DispareitorPersonaje->GetVelocity());
    // Calcula la desviacion entre ambos rotator
    FRotator DeltaRotacionObjetivo = UKismetMathLibrary::NormalizedDeltaRotator(RotacionMovimiento, RotacionApuntado);
    // Rotacion suave. Es parecido al Smoothing Time del BS, pero si el problema que conlleva su uso, y es que esta última interpola entre -180 a 180 pasando por 0, lo cual provoca un glitch cuando
    // nos movemos hacia atras. En cambio esta funcion, utiliza el camino mas corto entre -180 y 180 sin pasar por 0
    DeltaRotacion = FMath::RInterpTo(DeltaRotacion, DeltaRotacionObjetivo, DeltaTime, 6.f);
    GiroDesviacion = DeltaRotacion.Yaw;

    // Inclinacion tiene que ver entre el Giro del anterior frame y el actual
    PersonajeRotacionUltimoFrame = PersonajeRotacion;
    PersonajeRotacion = DispareitorPersonaje->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(PersonajeRotacion, PersonajeRotacionUltimoFrame);
    const float InclinacionObjetivo = Delta.Yaw / DeltaTime; // Lo escala y lo hacemos proporcional al DeltaTime
    const float Interpolacion = FMath::FInterpTo(Inclinacion, InclinacionObjetivo, DeltaTime, 6.f);  // Si la velocidad de interpolacion (ultimo parametro) es 0 devuelve InclinacionObjetivo
    Inclinacion = FMath::Clamp(Interpolacion, -90.f, 90.f);
}
