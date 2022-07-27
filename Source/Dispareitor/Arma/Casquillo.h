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
	UFUNCTION()	virtual void Callback_AlGolpear(UPrimitiveComponent* ComponenteGolpeante, AActor* ActorGolpeado, UPrimitiveComponent* ComponenteGolpeado, FVector ImpulsoNormal, const FHitResult& GolpeResultado);

private: 
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Malla;
	UPROPERTY(EditAnywhere)	float ImpulsoEyeccion;	
	UPROPERTY(EditAnywhere)	class USoundCue* Sonido;	

};
