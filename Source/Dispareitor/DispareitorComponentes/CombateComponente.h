#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombateComponente.generated.h"

#define RAYO_LONGITUD 80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCombateComponente : public UActorComponent {
	GENERATED_BODY()

public:		
	UCombateComponente();
	// Ya que estan relacionadas, definimos ADispareitorPersonaje como clase amiga para que pueda acceder a sus metodos y variables protegidas y privadas
	friend class ADispareitorPersonaje;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;			
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquiparArma(class AArma* ArmaAEquipar);

protected:	
	virtual void BeginPlay() override;
	void ActualizarApuntando(bool Apuntado);

	UFUNCTION(Server, Reliable)
	void ServidorActualizarApuntando(bool Apuntando);

	UFUNCTION()
	void AlReplicarArmaEquipada();

	void DispararPresionado(bool bPresionado);

	UFUNCTION(Server, Reliable)
	void ServidorDisparar(const FVector_NetQuantize& Objetivo);

	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor+ clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisparar(const FVector_NetQuantize& Objetivo);

	void CalcularRayoDesdeCruceta(FHitResult& RayoResultado);

	void ActualizarHUDCruceta(float DeltaTime);

private:
	class ADispareitorPersonaje* DispareitorPersonaje;
	class ADispareitorControladorJugador* DispareitorControladorJugador;
	class ADispareitorHUD* DispareitorHUD;

	UPROPERTY(ReplicatedUsing = AlReplicarArmaEquipada)
	AArma* ArmaEquipada;

	UPROPERTY(Replicated)
	bool bApuntando;

	UPROPERTY(EditAnywhere)
	float VelocidadCaminarBase;

	UPROPERTY(EditAnywhere)
	float VelocidadCaminarApuntando;

	bool bDispararPresionado;

	float CrucetaFactorVelocidad;
	float CrucetaFactorEnAire;

	FVector ObjetoAlcanzado;

	float PorDefectoFOV;

	UPROPERTY(EditAnywhere, Category = Combate)
	float ZoomFOV =  30.f;

	float ActualFOV;

	UPROPERTY(EditAnywhere, Category = Combate)
	float ZoomVelocidadInterpolacion = 20.f;

	void InterpolarFOV(float DeltaTime);
	
public:		
	
};
