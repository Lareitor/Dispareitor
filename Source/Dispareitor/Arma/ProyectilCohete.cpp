#include "ProyectilCohete.h"
#include "Kismet/GameplayStatics.h"

AProyectilCohete::AProyectilCohete() {
    Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
    Malla->SetupAttachment(RootComponent);
    Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProyectilCohete::CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    APawn* PeonQueDispara = GetInstigator();  
    if(PeonQueDispara) {
        AController* ControladorDeQuienDispara = PeonQueDispara->GetController();
        if(ControladorDeQuienDispara) {
            UGameplayStatics::ApplyRadialDamageWithFalloff(this, Dano, 10.f, GetActorLocation(), 200.f, 500.f, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, ControladorDeQuienDispara);
        }
    }
    
    Super::CallbackAlImpactar(ComponenteImpactante, ActorImpactado, ComponenteImpactado, ImpulsoNormal, ImpactoResultado);
}
