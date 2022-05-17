#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dispareitor/Tipos/GirarEnSitio.h"
#include "DispareitorPersonaje.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorPersonaje : public ACharacter {
	GENERATED_BODY()

public:	
	ADispareitorPersonaje();	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void EjecutarMontajeDispararArma(bool bApuntando);

protected:
	virtual void BeginPlay() override;
	void MoverAdelanteAtras(float Valor);
	void MoverIzquierdaDerecha(float Valor);
	void Girar(float Valor);
	void MirarArribaAbajo(float Valor);
	void Equipar();
	void Agachar();
	void ApuntarPulsado();
	void ApuntarLiberado();
	void CalcularDesplazamientoEnApuntado(float DeltaTime);
	virtual void Jump() override;
	void DispararPulsado();
	void DispararLiberado();

private:
	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class USpringArmComponent* BrazoCamara;

	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class UCameraComponent* Camara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HUDSobreLaCabeza;

	// Cuando cambie esta variable en el servidor se replicará su estado automaticamente en los clientes seleccionados
	// La replicación funciona sola en una direccion servidor -> clientes
	UPROPERTY(ReplicatedUsing = AlReplicarArmaSolapada) 
	class AArma* ArmaSolapada;
		
	UFUNCTION()
	void AlReplicarArmaSolapada(AArma* ArmaReplicadaAnterior);

	UPROPERTY(VisibleAnywhere)
	class UCombateComponente* CombateComponente;

	// Hace esta funcion de tipo RPC, para llamarla desde los clientes pero que se ejecuten en el servidor
	// Reliable garantiza que la información llega al servidor (el cliente recibe una confirmación de parte del servidor, si no la recibe vuelve a enviar la info)
	UFUNCTION(Server, Reliable) 
	void ServidorEquipar();

	float AOGiro;
	float InterpolacionAOGiro;
	float AOInclinacion;
	FRotator ArmadoRotacionInicial;

	EGirarEnSitio GirarEnSitio;
	void CalcularGirarEnSitio(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeDispararArma;

public:	
	void ActivarArmaSolapada(AArma* Arma);
	bool EstaArmaEquipada();
	bool EstaApuntando();
	FORCEINLINE float ObtenerAOGiro() const { return AOGiro; } 
	FORCEINLINE float ObtenerAOInclinacion() const { return AOInclinacion; } 
	AArma* ObtenerArmaEquipada();
	FORCEINLINE EGirarEnSitio ObtenerGirarEnSitio() const { return GirarEnSitio; }
	FVector ObtenerObjetoAlcanzado() const;
};
