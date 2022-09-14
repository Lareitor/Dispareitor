#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompensacionLagComponente.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCompensacionLagComponente : public UActorComponent {
	GENERATED_BODY()

public:	
	UCompensacionLagComponente();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
