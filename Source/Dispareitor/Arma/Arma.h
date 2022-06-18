#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "Arma.generated.h"

UENUM(BlueprintType)
enum class EEstado : uint8 {
	EEA_Inicial UMETA(DisplayName = "Inicial"),
	EEA_Equipada UMETA(DisplayName = "Equipada"),
	EEA_Desequipada UMETA(DisplayName = "Desequipada"),
	EEA_Maximo UMETA(DisplayName = "Maximo") // Para saber cuantos valores hay almacenados. Bastaría con obtener el valor numerico de este
};

UCLASS()
class DISPAREITOR_API AArma : public AActor {
	GENERATED_BODY()
	
public:		
	AArma();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void ActualizarHUDMunicion();
	void MostrarLeyendaSobreArma(bool bMostrarLeyendaSobreArma);
	virtual void Disparar(const FVector& Objetivo);
	void Soltar();
	void MunicionModificar(int32 Cantidad);

	UPROPERTY(EditAnywhere, Category = Cruceta)
	class UTexture2D* CrucetaCentro;

	UPROPERTY(EditAnywhere, Category = Cruceta)
	UTexture2D* CrucetaIzquierda;

	UPROPERTY(EditAnywhere, Category = Cruceta)
	UTexture2D* CrucetaDerecha;

	UPROPERTY(EditAnywhere, Category = Cruceta)
	UTexture2D* CrucetaArriba;

	UPROPERTY(EditAnywhere, Category = Cruceta)
	UTexture2D* CrucetaAbajo;

	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomVelocidadInterpolacion = 20.f;

	UPROPERTY(EditAnywhere, Category = Combate)
	float DisparoRetardo = 0.15f;

	UPROPERTY(EditAnywhere, Category = Combate)
	bool bAutomatica = true;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquiparSonido;

protected:	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void CallbackEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CallbackEsferaSolapadaFin(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice);

private:
	UPROPERTY(VisibleAnywhere, Category = Propiedades)
	USkeletalMeshComponent* Malla;

	UPROPERTY(VisibleAnywhere, Category = Propiedades)
	class USphereComponent* Esfera;

	UPROPERTY(ReplicatedUsing = AlReplicarEstado, VisibleAnywhere, Category = "Propiedades")
	EEstado Estado;

	UFUNCTION()
	void AlReplicarEstado();

	UPROPERTY(VisibleAnywhere, Category = Propiedades)
	class UWidgetComponent* LeyendaSobreArma;

	UPROPERTY(EditAnywhere, Category = Propiedades)
	class UAnimationAsset* AnimacionDisparar;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasquillo> Casquillo;

	UPROPERTY(EditAnywhere, ReplicatedUsing = AlReplicarMunicion)
	int32 Municion;

	UFUNCTION()
	void AlReplicarMunicion();

	void GastarMunicion();

	UPROPERTY(EditAnywhere)
	int32 CargadorCapacidad;

	UPROPERTY()
	class ADispareitorPersonaje* DispareitorPersonaje;

	UPROPERTY()
	class ADispareitorControladorJugador* DispareitorControladorJugador;

	UPROPERTY(EditAnywhere)
	ETipoArma TipoArma;
	

public:		
	void ActualizarEstado(EEstado EstadoAActualizar);
	FORCEINLINE USphereComponent* ObtenerEsfera() const { return Esfera; }
	FORCEINLINE USkeletalMeshComponent* ObtenerMalla() const { return Malla; }
	FORCEINLINE float ObtenerZoomFOV() const { return ZoomFOV; }
	FORCEINLINE float ObtenerVelocidadInterpolacion() const { return ZoomVelocidadInterpolacion; }
	bool EstaSinMunicion();
	FORCEINLINE ETipoArma TipoArmaObtener() const { return TipoArma; } 
	FORCEINLINE int32 MunicionObtener() const { return Municion; }
	FORCEINLINE int32 CargadorCapacidadObtener() const { return CargadorCapacidad; }
};
