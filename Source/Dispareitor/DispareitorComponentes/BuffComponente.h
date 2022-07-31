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
	void AumentarVelocidad(float VelocidadDePieAumentada, float VelocidadAgachadoAumentada, float Duracion);
	void InicializarVelocidadesOriginales(float _VelocidadDePieOriginal, float _VelocidadAgachadoOriginal);

protected:
	virtual void BeginPlay() override;
	void SanarProgreso(float DeltaTime);

private:
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	/**
	 * Vida buff
	 */
	bool bSanando = false;
	float RatioSanacion = 0.f;
	float IncrementoASanar = 0.f;
	/**
	 * Velocidad buff
	 * 
	 */
	FTimerHandle TemporizadorAumentoVelocidad;
	void ResetearVelocidades();
	float VelocidadDePieOriginal;
	float VelocidadAgachadoOriginal;
	UFUNCTION(NetMulticast, Reliable) void AumentarVelocidad_Multicast(float VelocidadDePieNueva, float VelocidadAgachadoNueva);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
