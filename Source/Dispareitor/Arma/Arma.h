#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arma.generated.h"

UENUM(BlueprintType)
enum class EEstado : uint8 {
	EEA_Inicial UMETA(DisplayName = "Inicial"),
	EEA_Equipada UMETA(DisplayName = "Equipada"),
	EEA_Desequipada UMETA(DisplayName = "Desequipada"),
	EEA_Max UMETA(DisplayName = "MAX") // Para saber cuantos valores hay almacenados. Bastaría con obtener el valor numerico de este
};

UCLASS()
class DISPAREITOR_API AArma : public AActor {
	GENERATED_BODY()
	
public:		
	AArma();
	virtual void Tick(float DeltaTime) override;

protected:	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void CallbackEsferaSolapada(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, Category = "Propiedades")
	USkeletalMeshComponent* Malla;

	UPROPERTY(VisibleAnywhere, Category = "Propiedades")
	class USphereComponent* Esfera;

	UPROPERTY(VisibleAnywhere, Category = "Propiedades")
	EEstado Estado;

	UPROPERTY(VisibleAnywhere, Category = "Propiedades")
	class UWidgetComponent* LeyendaSobreArma;

public:		
	

};
