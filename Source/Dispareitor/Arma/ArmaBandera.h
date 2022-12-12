#pragma once

#include "CoreMinimal.h"
#include "Arma.h"
#include "ArmaBandera.generated.h"

UCLASS()
class DISPAREITOR_API AArmaBandera : public AArma {
	GENERATED_BODY()

public:
	void Resetear();
	void PermitirResetearSiHaCaidoEnLimitesJuego();

protected:
	virtual void BeginPlay() override;	
	virtual void ManejarActualizacionEstadoAlEquipar() override;

private:
	FTransform TransformInicial;

public:
	FORCEINLINE FTransform ObtenerTransformInicial() const { return TransformInicial; }		
};
