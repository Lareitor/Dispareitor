#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponente.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UBuffComponente : public UActorComponent {
	GENERATED_BODY()

public:	
	UBuffComponente();
	friend class ADispareitorPersonaje;
	void Sanar(float IncrementoVida, float TiempoIncrementoVida);
	void Escudar(float IncrementoEscudo, float TiempoIncrementoEscudo);
	void AumentarVelocidad(float VelocidadDePieAumentada, float VelocidadAgachadoAumentada, float Duracion);
	void InicializarVelocidadesOriginales(float _VelocidadDePieOriginal, float _VelocidadAgachadoOriginal);
	void AumentarSalto(float SaltoAumentando, float Duracion);
	void InicializarSaltoOriginal(float _SaltoOriginal);

protected:
	virtual void BeginPlay() override;
	void SanarProgreso(float DeltaTime);
	void EscudarProgreso(float DeltaTime);

private:
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	/**
	 * Vida buff
	 */
	bool bSanando = false;
	float RatioSanacion = 0.f;
	float IncrementoASanar = 0.f;
	/**
	 * Escudo buff
	 */
	bool bEscudando = false;
	float RatioEscudacion = 0.f;
	float IncrementoAEscudar = 0.f;
	/**
	 * Velocidad buff
	 */
	FTimerHandle TemporizadorAumentoVelocidad;
	void ResetearVelocidades();
	float VelocidadDePieOriginal;
	float VelocidadAgachadoOriginal;
	UFUNCTION(NetMulticast, Reliable) void AumentarVelocidad_Multicast(float VelocidadDePieNueva, float VelocidadAgachadoNueva);
	/**
	 * Salto buff
	 */
	FTimerHandle TemporizadorAumentoSalto;
	void ResetearSalto();
	float SaltoOriginal;
	UFUNCTION(NetMulticast, Reliable) void AumentarSalto_Multicast(float SaltoNuevo);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
