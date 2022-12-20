#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "Dispareitor/Tipos/Equipo.h"
#include "Arma.generated.h"

UENUM(BlueprintType)
enum class EEstado : uint8 {
	EEA_Inicial UMETA(DisplayName = "Inicial"),
	EEA_Equipada UMETA(DisplayName = "Equipada"),
	EEA_EquipadaSecundaria UMETA(DisplayName = "Equipada Secundaria"),
	EEA_Desequipada UMETA(DisplayName = "Desequipada"),
	EEA_Maximo UMETA(DisplayName = "Maximo") // Para saber cuantos valores hay almacenados. Bastaría con obtener el valor numerico de este
};

UENUM(BlueprintType)
enum class ETipoDisparo : uint8 {
	ETD_HitScan UMETA(DisplayName = "Arma HitScan"),
	ETD_Proyectil UMETA(DisplayName = "Arma Proyectil"),
	ETD_Escopeta UMETA(DisplayName = "Arma Escopeta"), // Aunque técnicamente la escopeta es hitscan sus caracteristicas especiales la hacen merecedora de un tipo aparte
	ETD_Maximo UMETA(DisplayName = "Maximo") 
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
	FVector CalcularPuntoFinalConDispersion(const FVector& Objetivo);

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
	UPROPERTY(EditAnywhere)	ETipoDisparo TipoDisparo;
	UPROPERTY(EditAnywhere, Category = "Dispersion") bool bUsarDispersion = false; // subfusil y escopeta

	void ActualizarNombrePuntoReaparicion(FString Nombre);

	void DeshabilitarColisiones();
	void HabilitarColisiones();

protected:	
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	UPROPERTY()	class ADispareitorControladorJugador* DispareitorControladorJugador;
	virtual void BeginPlay() override;
	virtual void ManejarActualizacionEstado();
	virtual void ManejarActualizacionEstadoAlEquipar();
	virtual void ManejarActualizacionEstadoAlSoltar();
	virtual void ManejarActualizacionEstadoAlEquiparSecundaria();
	UFUNCTION()	virtual void Callback_EsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()	virtual void Callback_EsferaSolapadaFin(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice);
	UPROPERTY(EditAnywhere, Category = "Dispersion") float DistanciaAEsferaDeDispersion = 800.f;
	UPROPERTY(EditAnywhere, Category = "Dispersion") float RadioDeEsferaDeDispersion = 75.f;
	UPROPERTY(EditAnywhere)	float Danio = 20.f;	
	UPROPERTY(EditAnywhere)	float DanioEnCabeza = 40.f;	
	UPROPERTY(Replicated, EditAnywhere)	bool bRebobinarLadoServidor = false;
	UFUNCTION() void Callback_PingAlto(bool bPingAlto);	

private:
	UPROPERTY(VisibleAnywhere, Category = Propiedades) USkeletalMeshComponent* Malla;
	UPROPERTY(VisibleAnywhere, Category = Propiedades) class USphereComponent* Esfera;
	UPROPERTY(ReplicatedUsing = AlReplicar_Estado, VisibleAnywhere, Category = "Propiedades") EEstado Estado;
	UFUNCTION()	void AlReplicar_Estado();
	UPROPERTY(VisibleAnywhere, Category = Propiedades) class UWidgetComponent* LeyendaSobreArma;
	UPROPERTY(EditAnywhere, Category = Propiedades)	class UAnimationAsset* AnimacionDisparar;
	UPROPERTY(EditAnywhere)	TSubclassOf<class ACasquillo> ClaseCasquillo; 
	UPROPERTY(EditAnywhere) int32 Municion;
	UFUNCTION(Client, Reliable) void ActualizarMunicion_EnCliente(int32 MunicionEnServidor);
	UFUNCTION(Client, Reliable) void AniadirMunicion_EnCliente(int32 MunicionEnServidor);
	// El nº de peticiones al servidor no procesadas para la Municion. 
	// Incrementadas en GastarMunicion, decrementandas en ActualizarMunicion_EnCliente
	int32 Secuencia = 0; 
	void GastarMunicion();
	UPROPERTY(EditAnywhere)	int32 CapacidadCargador;  
	UPROPERTY(EditAnywhere)	ETipoArma TipoArma;
	UPROPERTY(EditAnywhere)	EEquipo Equipo;
	int bReseteando = false; // Se usa para cuando el arma cae en LimitesJuego
	UPROPERTY(Replicated, EditAnywhere) FString NombrePuntoReaparicion;	
	
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
	FORCEINLINE float ObtenerDanio() const { return Danio; }
	FORCEINLINE float ObtenerDanioEnCabeza() const { return DanioEnCabeza; }
	FORCEINLINE EEquipo ObtenerEquipo() const { return Equipo; }	
	FORCEINLINE FString ObtenerNombrePuntoReaparicion() const { return NombrePuntoReaparicion; }
};
