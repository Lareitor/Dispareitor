#include "ProyectilBala.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Hereda de AProyectil

AProyectilBala::AProyectilBala() {
    ProyectilMovimientoComponente = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProyectilMovimientoComponente"));
	ProyectilMovimientoComponente->bRotationFollowsVelocity = true;
    ProyectilMovimientoComponente->SetIsReplicated(true);
}

void AProyectilBala::CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    ACharacter* PersonajePropietario = Cast<ACharacter>(GetOwner());
    if(PersonajePropietario) {
        AController* ControladorPropietario = PersonajePropietario->Controller;
        if(ControladorPropietario) {
             UGameplayStatics::ApplyDamage(ActorImpactado, Dano, ControladorPropietario, this, UDamageType::StaticClass());
        }
    }
    
    // Lo llamamos al final porque el Super invoca a Destroy y por tanto no se ejecutaría el codigo que pongamos a continuación de esta linea
    Super::CallbackAlImpactar(ComponenteImpactante, ActorImpactado, ComponenteImpactado, ImpulsoNormal, ImpactoResultado);    
}
