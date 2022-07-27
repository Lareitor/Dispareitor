#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DispareitorEstadoJugador.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorEstadoJugador : public APlayerState {
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnRep_Score() override; // Como ya viene implementada de serie nos sirve para cambiar los muertos (a favor)
	UFUNCTION()	virtual void AlReplicar_Muertes(); // Esta la implementamos nosotros para las muertes (en contra) 
	void IncrementarMuertos(float Incremento);
	void IncrementarMuertes(int32 Incremento);

private:	
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	UPROPERTY()	class ADispareitorControladorJugador* DispareitorControladorJugador;
	UPROPERTY(ReplicatedUsing = AlReplicar_Muertes)	int32 Muertes;	
};
