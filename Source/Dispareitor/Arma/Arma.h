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
	void ActualizarMunicionHUD();
	void MostrarLeyendaSobreArma(bool bMostrarLeyendaSobreArma);
	virtual void Disparar(const FVector& Objetivo);
	void Soltar();
	void AniadirMunicion(int32 Cantidad);
	UPROPERTY(EditAnywhere, Category = Cruceta)	class UTexture2D* CrucetaCentro;
	UPROPERTY(EditAnywhere, Category = Cruceta)	UTexture2D* CrucetaIzquierda;
	UPROPERTY(EditAnywhere, Category = Cruceta)	UTexture2D* CrucetaDerecha;
	UPROPERTY(EditAnywhere, Category = Cruceta)	UTexture2D* CrucetaArriba;
	UPROPERTY(EditAnywhere, Category = Cruceta)	UTexture2D* CrucetaAbajo;
	UPROPERTY(EditAnywhere)	float ZoomFOV = 30.f;
	UPROPERTY(EditAnywhere)	float ZoomVelocidadInterpolacion = 20.f;
	UPROPERTY(EditAnywhere, Category = Combate)	float RetardoAlDisparar = 0.15f; //escopeta: 1,1  franco: 1,5 lanzacohetes: 1  lanzagranadas: 1  pistola: 0,15  asalto: 0,15 subfusil: 0,1   
	UPROPERTY(EditAnywhere, Category = Combate)	bool bAutomatica = true;
	UPROPERTY(EditAnywhere)	class USoundCue* SonidoEquipar; 
	void PermitirProfundidadPersonalizadaAlRenderizar(bool bPermitir);
	bool bDestruirArma = false;

protected:	
	virtual void BeginPlay() override;
	UFUNCTION()	virtual void Callback_EsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()	virtual void Callback_EsferaSolapadaFin(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice);

private:
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	UPROPERTY()	class ADispareitorControladorJugador* DispareitorControladorJugador;
	UPROPERTY(VisibleAnywhere, Category = Propiedades) USkeletalMeshComponent* Malla;
	UPROPERTY(VisibleAnywhere, Category = Propiedades) class USphereComponent* Esfera;
	UPROPERTY(ReplicatedUsing = AlReplicar_Estado, VisibleAnywhere, Category = "Propiedades") EEstado Estado;
	UFUNCTION()	void AlReplicar_Estado();
	UPROPERTY(VisibleAnywhere, Category = Propiedades) class UWidgetComponent* LeyendaSobreArma;
	UPROPERTY(EditAnywhere, Category = Propiedades)	class UAnimationAsset* AnimacionDisparar;
	UPROPERTY(EditAnywhere)	TSubclassOf<class ACasquillo> ClaseCasquillo; 
	UPROPERTY(EditAnywhere, ReplicatedUsing = AlReplicar_Municion) int32 Municion;
	UFUNCTION()	void AlReplicar_Municion();
	void GastarMunicion();
	UPROPERTY(EditAnywhere)	int32 CapacidadCargador;  
	UPROPERTY(EditAnywhere)	ETipoArma TipoArma;
	
public:		
	void ActualizarEstado(EEstado EstadoAActualizar);
	FORCEINLINE USphereComponent* ObtenerEsfera() const { return Esfera; }
	FORCEINLINE USkeletalMeshComponent* ObtenerMalla() const { return Malla; }
	FORCEINLINE float ObtenerZoomFOV() const { return ZoomFOV; }
	FORCEINLINE float ObtenerVelocidadInterpolacion() const { return ZoomVelocidadInterpolacion; }
	bool EstaSinMunicion();
	bool EstaConMunicionLlena();
	FORCEINLINE ETipoArma ObtenerTipoArma() const { return TipoArma; } 
	FORCEINLINE int32 ObtenerMunicion() const { return Municion; }
	FORCEINLINE int32 ObtenerCapacidadCargador() const { return CapacidadCargador; }
};
