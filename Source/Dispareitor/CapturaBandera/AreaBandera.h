#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dispareitor/Tipos/Equipo.h"
#include "AreaBandera.generated.h"

UCLASS()
class DISPAREITOR_API AAreaBandera : public AActor {
	GENERATED_BODY()
	
public:	
	AAreaBandera();
	UPROPERTY(EditAnywhere) EEquipo Equipo;	

protected:	
	virtual void BeginPlay() override;
	UFUNCTION()	virtual void Callback_EsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);	

private:
	UPROPERTY(EditAnywhere)	 class USphereComponent* Area;
	
};
