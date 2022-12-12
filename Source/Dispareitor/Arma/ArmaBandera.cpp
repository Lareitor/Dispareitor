#include "ArmaBandera.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h" 

void AArmaBandera::BeginPlay() {
    Super::BeginPlay();
    TransformInicial = GetActorTransform();
}

void AArmaBandera::ManejarActualizacionEstadoAlEquipar() {
    MostrarLeyendaSobreArma(false);
	ObtenerEsfera()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ObtenerMalla()->SetSimulatePhysics(false);
	ObtenerMalla()->SetEnableGravity(false);
	ObtenerMalla()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ObtenerMalla()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);	
	PermitirProfundidadPersonalizadaAlRenderizar(false);
}

void AArmaBandera::PermitirResetearSiHaCaidoEnLimitesJuego() {
    ManejarActualizacionEstadoAlEquipar();  
}


void AArmaBandera::Resetear() {    
    ADispareitorPersonaje* DPersonajePortadorBandera = Cast<ADispareitorPersonaje>(GetOwner());
    if(DPersonajePortadorBandera) {
        DPersonajePortadorBandera->ActualizarSosteniendoBandera(false);
        DPersonajePortadorBandera->ActivarArmaSolapada(nullptr);
        DPersonajePortadorBandera->UnCrouch();
    }

    if(HasAuthority()) {
        FDetachmentTransformRules DesvincularReglas(EDetachmentRule::KeepWorld, true);
        ObtenerMalla()->DetachFromComponent(DesvincularReglas);

        ActualizarEstado(EEstado::EEA_Inicial);
        ObtenerEsfera()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        ObtenerEsfera()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);	

        SetOwner(nullptr);
        DispareitorPersonaje = nullptr;
        DispareitorControladorJugador = nullptr;

        SetActorTransform(TransformInicial);
    }    
}
