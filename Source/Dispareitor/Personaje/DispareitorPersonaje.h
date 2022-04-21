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

protected:
	virtual void BeginPlay() override;

	void MoverAdelanteAtras(float Valor);
	void MoverIzquierdaDerecha(float Valor);
	void Girar(float Valor);
	void MoverArribaAbajo(float Valor);

private:
	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class USpringArmComponent* BrazoCamara;

	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class UCameraComponent* Camara;

public:	
	
};
