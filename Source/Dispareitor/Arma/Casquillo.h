#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casquillo.generated.h"

UCLASS()
class DISPAREITOR_API ACasquillo : public AActor {
	GENERATED_BODY()
	
public:	
	ACasquillo();

protected:
	virtual void BeginPlay() override;

private: 
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Malla;		

};
