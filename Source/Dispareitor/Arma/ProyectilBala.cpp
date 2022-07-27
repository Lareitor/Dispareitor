#include "ProyectilBala.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Hereda de AProyectil

AProyectilBala::AProyectilBala() {
    ComponenteMovimientoProyectil = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ComponenteMovimientoProyectil"));
	ComponenteMovimientoProyectil->bRotationFollowsVelocity = true;
    ComponenteMovimientoProyectil->SetIsReplicated(true);
}

void AProyectilBala::Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    ACharacter* PersonajePropietario = Cast<ACharacter>(GetOwner());
    if(PersonajePropietario) {
        AController* ControladorPropietario = PersonajePropietario->Controller;
        if(ControladorPropietario) {
             UGameplayStatics::ApplyDamage(ActorImpactado, Danio, ControladorPropietario, this, UDamageType::StaticClass());
        }
    }
    
    // Lo llamamos al final porque el Super invoca a Destroy y por tanto no se ejecutaría el codigo que pongamos a continuación de esta linea
    Super::Callback_AlImpactar(ComponenteImpactante, ActorImpactado, ComponenteImpactado, ImpulsoNormal, ImpactoResultado);    
}
