#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Dispareitor/Tipos/Equipo.h"
#include "InicioJugadorEquipo.generated.h"

UCLASS()
class DISPAREITOR_API AInicioJugadorEquipo : public APlayerStart {
	GENERATED_BODY()	
	
public:
	UPROPERTY(EditAnywhere) EEquipo Equipo;	
};
