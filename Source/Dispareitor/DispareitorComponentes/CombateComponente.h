#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombateComponente.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCombateComponente : public UActorComponent {
	GENERATED_BODY()

public:		
	UCombateComponente();
	// Ya que estan relacionadas, definimos ADispareitorPersonaje como clase amiga para que pueda acceder a sus metodos y variables protegidas y privadas
	friend class ADispareitorPersonaje;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;			
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquiparArma(class AArma* ArmaAEquipar);

protected:	
	virtual void BeginPlay() override;
	void ActualizarApuntando(bool Apuntado);

	UFUNCTION(Server, Reliable)
	void ServidorActualizarApuntando(bool Apuntando);

private:
	class ADispareitorPersonaje* DispareitorPersonaje;

	UPROPERTY(Replicated)
	AArma* ArmaEquipada;

	UPROPERTY(Replicated)
	bool bApuntando;

public:		
	
};
