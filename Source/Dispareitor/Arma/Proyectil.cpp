#include "Proyectil.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/Dispareitor.h"

AProyectil::AProyectil() {
	PrimaryActorTick.bCanEverTick = true;
	// El proyectil es creado en el servidor pero como tenemos esta variable a true, es replicada en los clientes tambien
	bReplicates = true;

	CajaColision = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaColision"));
	SetRootComponent(CajaColision);
	CajaColision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CajaColision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CajaColision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CajaColision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CajaColision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	// Ya que tanto la capsula como la malla del personaje estan en el canal Pawn, y queremos ajustar los disparos a la malla, lo que hacemos es crear un canal especifico para la malla y colisionar con él
	CajaColision->SetCollisionResponseToChannel(ECC_MallaDelEsqueleto, ECollisionResponse::ECR_Block);
}

void AProyectil::BeginPlay() {
	Super::BeginPlay();

	if(SistemaParticulasTraza) {
		ComponenteSistemaParticulasTraza = UGameplayStatics::SpawnEmitterAttached(SistemaParticulasTraza, CajaColision, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	// El servidor es el que se encarga de manejar la colision
	if(HasAuthority()) {
		CajaColision->OnComponentHit.AddDynamic(this, &AProyectil::Callback_AlImpactar);
	}
}

void AProyectil::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AProyectil::Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
	// Al llamar a este metodo se propaga a todos los clientes y se invoca a Destroyed, y es ahí donde realizamos el efecto de particulas y sonido
	Destroy();
}

void AProyectil::Destroyed() {
	Super::Destroyed();

	if(SistemaParticulasAlImpactar) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasAlImpactar, GetActorTransform());
	}
	if(SonidoAlImpactar) {
		UGameplayStatics::PlaySoundAtLocation(this, SonidoAlImpactar, GetActorLocation());
	}
}

void AProyectil::CrearTrazaDeHumo() {
	if(SistemaNiagaraTrazaDeHumo) {
        ComponenteNiagaraTrazaDeHumo = UNiagaraFunctionLibrary::SpawnSystemAttached(SistemaNiagaraTrazaDeHumo, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
    }
}

void AProyectil::IniciarTemporizadorExplosion() {
	GetWorldTimerManager().SetTimer(TemporizadorExplosion, this, &AProyectil::EjecutarExplosion, TiempoParaExplosion);
}

void AProyectil::EjecutarExplosion() {
    AplicarDanioDeExplosion();
    Destroy();
}

void AProyectil::AplicarDanioDeExplosion() {  
    APawn* PeonQueDispara = GetInstigator();  
    if(PeonQueDispara && HasAuthority()) {
        AController* ControladorDeQuienDispara = PeonQueDispara->GetController();
        if(ControladorDeQuienDispara) {
            UGameplayStatics::ApplyRadialDamageWithFalloff(this, Danio, 10.f, GetActorLocation(), RadioInternoDeDanio, RadioExternoDeDanio, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, ControladorDeQuienDispara);
        }
    }
}



