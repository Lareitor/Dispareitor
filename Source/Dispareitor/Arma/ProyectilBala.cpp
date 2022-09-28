#include "ProyectilBala.h"
#include "Kismet/GameplayStatics.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Dispareitor/DispareitorComponentes/CompensacionLagComponente.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Hereda de AProyectil

AProyectilBala::AProyectilBala() {
    ComponenteMovimientoProyectil = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ComponenteMovimientoProyectil"));
	ComponenteMovimientoProyectil->bRotationFollowsVelocity = true;
    ComponenteMovimientoProyectil->SetIsReplicated(true);
    ComponenteMovimientoProyectil->InitialSpeed = VelocidadInicial;
    ComponenteMovimientoProyectil->MaxSpeed = VelocidadInicial;
}

#if WITH_EDITOR
void AProyectilBala::PostEditChangeProperty(struct FPropertyChangedEvent& Evento) {
    Super::PostEditChangeProperty(Evento);

    FName NombrePropiedad = Evento.Property != nullptr ? Evento.Property->GetFName() : NAME_None;
    if(NombrePropiedad == GET_MEMBER_NAME_CHECKED(AProyectilBala, VelocidadInicial) ) {
        if(ComponenteMovimientoProyectil) {
            ComponenteMovimientoProyectil->InitialSpeed = VelocidadInicial;
            ComponenteMovimientoProyectil->MaxSpeed = VelocidadInicial;
        }    
    }
}
#endif

void AProyectilBala::BeginPlay() {
    Super::BeginPlay();

    /*
    FPredictProjectilePathParams ParametrosPredecirTrayectoriaProyectil;
    ParametrosPredecirTrayectoriaProyectil.bTraceWithChannel = true; // Calcular trayectoria con un canal especifico 
    ParametrosPredecirTrayectoriaProyectil.bTraceWithCollision = true; // Nos permite generar eventos de colision
    ParametrosPredecirTrayectoriaProyectil.DrawDebugTime = 5.f; // Dibujar la trayectoria durante 5 sg.
    ParametrosPredecirTrayectoriaProyectil.DrawDebugType = EDrawDebugTrace::ForDuration; // Para la duracion de 5 sg.
    ParametrosPredecirTrayectoriaProyectil.LaunchVelocity = GetActorForwardVector() * VelocidadInicial;
    ParametrosPredecirTrayectoriaProyectil.MaxSimTime = 4.f; // Tiempo máximo que el proyectil será simulado volando
    ParametrosPredecirTrayectoriaProyectil.ProjectileRadius = 5.f;
    ParametrosPredecirTrayectoriaProyectil.SimFrequency = 30.f; // Como de real es la simulación, cuanto mayor sea el nº más exacta será
    ParametrosPredecirTrayectoriaProyectil.StartLocation = GetActorLocation();
    ParametrosPredecirTrayectoriaProyectil.TraceChannel = ECollisionChannel::ECC_Visibility;
    ParametrosPredecirTrayectoriaProyectil.ActorsToIgnore.Add(this);

    FPredictProjectilePathResult ResultadoPredecirTrayectoriaProyectil;
    UGameplayStatics::PredictProjectilePath(this, ParametrosPredecirTrayectoriaProyectil, ResultadoPredecirTrayectoriaProyectil);
    */
}

void AProyectilBala::Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    ADispareitorPersonaje* PersonajePropietario = Cast<ADispareitorPersonaje>(GetOwner());
    if(PersonajePropietario) {
        ADispareitorControladorJugador* ControladorPropietario = Cast<ADispareitorControladorJugador>(PersonajePropietario->Controller);
        if(PersonajePropietario->HasAuthority() && !bRebobinarLadoServidor) {
            UGameplayStatics::ApplyDamage(ActorImpactado, Danio, ControladorPropietario, this, UDamageType::StaticClass());
        } else {
            ADispareitorPersonaje* PersonajeImpactado = Cast<ADispareitorPersonaje>(ActorImpactado);
            if(PersonajePropietario->IsLocallyControlled() && bRebobinarLadoServidor && PersonajePropietario->ObtenerCompensacionLagComponente() && PersonajeImpactado) {
                PersonajePropietario->ObtenerCompensacionLagComponente()->PeticionImpactoProyectil_EnServidor(PersonajeImpactado, InicioRayo, VectorVelocidadInicial, ControladorPropietario->ObtenerTiempoServidor() -  ControladorPropietario->STT);
            }
        }        
    }

    // Lo llamamos al final porque el Super invoca a Destroy y por tanto no se ejecutaría el codigo que pongamos a continuación de esta linea
    Super::Callback_AlImpactar(ComponenteImpactante, ActorImpactado, ComponenteImpactado, ImpulsoNormal, ImpactoResultado);
}
