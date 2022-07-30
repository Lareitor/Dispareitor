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

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
