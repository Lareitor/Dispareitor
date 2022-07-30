#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponente.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UBuffComponente : public UActorComponent {
	GENERATED_BODY()

public:	
	UBuffComponente();
	friend class ADispareitorPersonaje;
	void Sanar(float IncrementoVida, float TiempoIncrementoVida);

protected:
	virtual void BeginPlay() override;
	void SanarProgreso(float DeltaTime);

private:
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	bool bSanando = false;
	float RatioSanacion = 0.f;
	float IncrementoASanar = 0.f;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
