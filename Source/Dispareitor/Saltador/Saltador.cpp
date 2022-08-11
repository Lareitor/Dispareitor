#include "Saltador.h"
#include "Components/BoxComponent.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/DispareitorComponentes/BuffComponente.h"

ASaltador::ASaltador() {
    ComponenteCaja = CreateDefaultSubobject<UBoxComponent>(TEXT("ComponenteCaja"));
	ComponenteCaja->SetupAttachment(RootComponent);
    ComponenteCaja->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ComponenteCaja->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ComponenteCaja->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ASaltador::BeginPlay(){
	Super::BeginPlay();

	ComponenteCaja->OnComponentBeginOverlap.AddDynamic(this, &ASaltador::Callback_ComponenteCajaSolapadaInicio);
}

void ASaltador::Callback_ComponenteCajaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {
    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
    if(DispareitorPersonaje) {
        UBuffComponente* BuffComponente = DispareitorPersonaje->ObtenerBuffComponente();
        if(BuffComponente) {
            BuffComponente->AumentarSalto(IncrementoSalto, Duracion);
        }
        DispareitorPersonaje->Jump();
    }
}