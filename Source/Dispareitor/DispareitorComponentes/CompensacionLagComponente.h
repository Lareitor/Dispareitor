#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompensacionLagComponente.generated.h"

USTRUCT(BlueprintType)
struct FCajaImpacto {
	GENERATED_BODY()

	UPROPERTY() FVector Posicion;
	UPROPERTY()	FRotator Rotacion;
	UPROPERTY()	FVector CajaExtension;
};

USTRUCT(BlueprintType)
struct FCajasImpactoFrame {
	GENERATED_BODY()

	UPROPERTY() float Tiempo;
	UPROPERTY()	TMap<FName, FCajaImpacto> CajasImpacto;
	UPROPERTY() ADispareitorPersonaje* DispareitorPersonaje;
};

USTRUCT(BlueprintType)
struct FResultadoRebobinarLadoServidor {
	GENERATED_BODY()

	UPROPERTY() bool bImpactoConfirmado;
	UPROPERTY() bool bTiroALaCabeza;
};

USTRUCT(BlueprintType)
struct FResultadoRebobinarLadoServidorEscopeta {
	GENERATED_BODY()

	UPROPERTY() TMap<ADispareitorPersonaje*,  uint32> TirosALaCabeza;
	UPROPERTY() TMap<ADispareitorPersonaje*,  uint32> TirosAlCuerpo;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCompensacionLagComponente : public UActorComponent {
	GENERATED_BODY()

public:	
	UCompensacionLagComponente();
	friend class ADispareitorPersonaje;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void MostrarCajasImpactoFrame(const FCajasImpactoFrame& CajasImpactoFrame, const FColor Color);
	FResultadoRebobinarLadoServidor RebobinarLadoServidorHitscan(class ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo, float TiempoImpacto);
	FResultadoRebobinarLadoServidor RebobinarLadoServidorProyectil(class ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize100& VelocidadInicial, float TiempoImpacto);
	FResultadoRebobinarLadoServidorEscopeta RebobinarLadoServidorEscopeta(const TArray<ADispareitorPersonaje*>& DispareitorPersonajesImpactados, const FVector_NetQuantize& InicioRayo, const TArray<FVector_NetQuantize>& ImpactosRayos, float TiempoImpacto);
	UFUNCTION(Server, Reliable) void PeticionImpacto_EnServidor(ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo, float TiempoImpacto, class AArma* ArmaCausanteDanio);
	UFUNCTION(Server, Reliable) void PeticionImpactoEscopeta_EnServidor(const TArray<ADispareitorPersonaje*>& DispareitorPersonajesImpactados, const FVector_NetQuantize& InicioRayo, const TArray<FVector_NetQuantize>& ImpactosRayos, float TiempoImpacto);


protected:
	virtual void BeginPlay() override;
	void GuardarCajasImpactoFrame(FCajasImpactoFrame& CajasImpactoFrame);
	FCajasImpactoFrame InterpolacionEntreFrames(const FCajasImpactoFrame& CajasImpactoFrameMasJoven, const FCajasImpactoFrame& CajasImpactoFrameMasViejo, float TiempoImpacto);
	void CachearCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado, FCajasImpactoFrame& CajasImpactoFrameSalida);
	void MoverCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado,const FCajasImpactoFrame& CajasImpactoFrame);
	void RestaurarCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado,const FCajasImpactoFrame& CajasImpactoFrame);
	void ModificarColisionMallaPersonaje(ADispareitorPersonaje* DispareitorPersonajeImpactado, ECollisionEnabled::Type TipoColisionPermitida);
	void GuardarCajasImpactoFrame();
	FCajasImpactoFrame ObtenerCajasImpactoFrameAComprobar(ADispareitorPersonaje* DispareitorPersonajeImpactado, float TiempoImpacto);
	FResultadoRebobinarLadoServidor ConfirmarImpactoHitscan(const FCajasImpactoFrame& CajasImpactoFrame, ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo);
	FResultadoRebobinarLadoServidor ConfirmarImpactoProyectil(const FCajasImpactoFrame& CajasImpactoFrame, ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize100& VelocidadInicial, float TiempoImpacto);
	FResultadoRebobinarLadoServidorEscopeta ConfirmarImpactoEscopeta(const TArray<FCajasImpactoFrame>& ArrayCajasImpactoFrame, const FVector_NetQuantize& InicioRayo, const TArray<FVector_NetQuantize>& ImpactosRayos);

private:
	UPROPERTY() ADispareitorPersonaje* DispareitorPersonaje;	
	UPROPERTY() class ADispareitorControladorJugador* DispareitorControladorJugador;
	TDoubleLinkedList<FCajasImpactoFrame> CajasImpactoFrames;
	UPROPERTY(EditAnywhere) float TiempoAlmacenamientoMaximo = 4.f;
};
