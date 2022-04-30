#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:
	virtual void BeginPlay() override;
	void MoverAdelanteAtras(float Valor);
	void MoverIzquierdaDerecha(float Valor);
	void Girar(float Valor);
	void MirarArribaAbajo(float Valor);
	void Equipar();

private:
	UPROPERTY(VisibleAnywhere, Category = "Camara")	
	class USpringArmComponent* BrazoCamara;

	UPROPERTY(VisibleAnywhere, Category = "Camara")	
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

public:	
	void ActivarArmaSolapada(AArma* Arma);
	
};
