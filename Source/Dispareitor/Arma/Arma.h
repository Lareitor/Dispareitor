#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void MostrarLeyendaSobreArma(bool bMostrarLeyendaSobreArma);
	virtual void Disparar(const FVector& Objetivo);

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

public:		
	void ActualizarEstado(EEstado EstadoAActualizar);
	FORCEINLINE USphereComponent* ObtenerEsfera() const { return Esfera; }
	FORCEINLINE USkeletalMeshComponent* ObtenerMalla() const { return Malla; }
};
