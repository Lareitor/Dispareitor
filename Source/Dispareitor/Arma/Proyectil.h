#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Proyectil.generated.h"

UCLASS()
class DISPAREITOR_API AProyectil : public AActor {
	GENERATED_BODY()
	
public:	
	AProyectil();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	bool bRebobinarLadoServidor = false;
	FVector_NetQuantize InicioRayo;
	FVector_NetQuantize100 VectorVelocidadInicial; // FVector_NetQuantize100 contiene dos decimales de precision, los necesitamos para que la velocidad sea precisa
	UPROPERTY(EditAnywhere) float VelocidadInicial = 15000.f;
	float Danio = 100.f; // bp_granadamano = 100

protected:
	virtual void BeginPlay() override;
	void IniciarTemporizadorExplosion();
	void EjecutarExplosion();
	void CrearTrazaDeHumo();
	void AplicarDanioDeExplosion();
	UFUNCTION()	virtual void Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado);
	UPROPERTY(EditAnywhere)	class UParticleSystem* SistemaParticulasAlImpactar; // BP_Proyectil: P_Impact_Metal_Large_01# BP_ProyectilBala: P_Impact_Metal_Large_01# BP_ProyectilGranada: P_Grenade_Explosion_01# BP_GranadaMano: P_Grenade_Explosion_01# BP_ProyectilCohete: P_RocketLauncher_Explosion_01 
	UPROPERTY(EditAnywhere)	class USoundCue* SonidoAlImpactar; // BP_Proyectil: Rifle_ImpactSurface_Cue# BP_ProyectilBala:Rifle_ImpactSurface_Cue # BP_ProyectilGranada: GrenadeLauncher_Explosion_Cue # BP_GranadaMano: GrenadeLauncher_Explosion_Cue# BP_ProyectilCohete: RocketLauncher_Explosion_Cue
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaColision;
	UPROPERTY(VisibleAnywhere) class UProjectileMovementComponent* ComponenteMovimientoProyectil;
	UPROPERTY(EditAnywhere)	class UNiagaraSystem* SistemaNiagaraTrazaDeHumo; // BP_Proyectil: Nada # BP_ProyectilBala: Nada # BP_ProyectilGranada: NS_HumoTrazaGranada# BP_GranadaMano: NS_HumoTrazaGranada# BP_ProyectilCohete: NS_HumoTraza
	UPROPERTY()	class UNiagaraComponent* ComponenteNiagaraTrazaDeHumo;  
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Malla;
	UPROPERTY(EditAnywhere)	float RadioInternoDeDanio = 200.f; // BP_Proyectil: Nada # BP_ProyectilBala: Nadad # BP_ProyectilGranada: 400 # BP_GranadaMano:  400# BP_ProyectilCohete:
	UPROPERTY(EditAnywhere)	float RadioExternoDeDanio = 500.f; // BP_Proyectil: Nada # BP_ProyectilBala: Nada # BP_ProyectilGranada: 800 # BP_GranadaMano: 800 # BP_ProyectilCohete:

private:
	UPROPERTY(EditAnywhere)	UParticleSystem* SistemaParticulasTraza; // BP_Proyectil: P_AssaultRifle_Tracer_01 # BP_ProyectilBala: P_AssaultRifle_Tracer_01 # BP_ProyectilGranada: Nada # BP_GranadaMano: Nada# BP_ProyectilCohete: 
	UPROPERTY()	class UParticleSystemComponent* ComponenteSistemaParticulasTraza;
	FTimerHandle TemporizadorExplosion;
	UPROPERTY(EditAnywhere) float TiempoParaExplosion = 3.f; // BP_Proyectil: # BP_ProyectilBala: # BP_ProyectilGranada: 1 # BP_GranadaMano: 2# BP_ProyectilCohete:  

};
