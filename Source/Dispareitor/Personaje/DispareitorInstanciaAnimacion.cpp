// Fill out your copyright notice in the Description page of Project Settings.


#include "DispareitorInstanciaAnimacion.h"
#include "DispareitorPersonaje.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Dispareitor/Arma/Arma.h"

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
    ArmaEquipada = DispareitorPersonaje->ObtenerArmaEquipada();
    bAgachado = DispareitorPersonaje->bIsCrouched;
    bApuntando = DispareitorPersonaje->EstaApuntando();
    GirarEnSitio = DispareitorPersonaje->ObtenerGirarEnSitio();

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

    AOGiro = DispareitorPersonaje->ObtenerAOGiro();
    AOInclinacion = DispareitorPersonaje->ObtenerAOInclinacion();

    if(bArmaEquipada && ArmaEquipada && ArmaEquipada->ObtenerMalla() && DispareitorPersonaje->GetMesh()) {
        // Obtener ManoIzquierdaTransform en espacio de mundo a partir de ManoIzquierdaSocket (arma)
        ManoIzquierdaTransform = ArmaEquipada->ObtenerMalla()->GetSocketTransform(FName("ManoIzquierdaSocket"), ERelativeTransformSpace::RTS_World);
        // Despues de llamar a TransformToBoneSpace, estas variables contendrán la posicion y rotacion de ManoIzquierdaSocket (arma) transformada a posicion relativa en espacio de huesos de nuestra hand_r (personaje)
        FVector PosicionSalida;
        FRotator RotacionSalida;
        DispareitorPersonaje->GetMesh()->TransformToBoneSpace(FName("hand_r"), ManoIzquierdaTransform.GetLocation(), FRotator::ZeroRotator, PosicionSalida, RotacionSalida);
        ManoIzquierdaTransform.SetLocation(PosicionSalida);
        ManoIzquierdaTransform.SetRotation(FQuat(RotacionSalida));

        // Como la rotacion de la mano derecha para que apunte en la direccion de la cruceta es solo cosmetica, solo lo aplicamos si somos el player local, no lo replicamos para ahorra ancho de banda
        if(DispareitorPersonaje->IsLocallyControlled()) {
            bControladoLocalmente = true;
            FTransform ManoDerechaTransform = ArmaEquipada->ObtenerMalla()->GetSocketTransform(FName("foo"), ERelativeTransformSpace::RTS_World);
            //FTransform ManoDerechaTransform = DispareitorPersonaje->GetMesh()->GetBoneTransform(DispareitorPersonaje->GetMesh()->GetBoneIndex(FName("hand_r")));  // GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
            ManoDerechaRotacion = UKismetMathLibrary::FindLookAtRotation(ManoDerechaTransform.GetLocation(), ManoDerechaTransform.GetLocation() + (ManoDerechaTransform.GetLocation() - DispareitorPersonaje->ObtenerObjetoAlcanzado()) );
 
            /*FTransform ArmaBocaTransform = ArmaEquipada->ObtenerMalla()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
            FVector ArmaBocaX(FRotationMatrix(ArmaBocaTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
            DrawDebugLine(GetWorld(), ArmaBocaTransform.GetLocation(), ArmaBocaTransform.GetLocation() + ArmaBocaX * 1000.f, FColor::Red);
            DrawDebugLine(GetWorld(), ArmaBocaTransform.GetLocation(), DispareitorPersonaje->ObtenerObjetoAlcanzado(), FColor::Green);*/ 
        }
    }
}
