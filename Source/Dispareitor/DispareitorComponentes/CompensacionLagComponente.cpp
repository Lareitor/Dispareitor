#include "CompensacionLagComponente.h"

UCompensacionLagComponente::UCompensacionLagComponente() {
	PrimaryComponentTick.bCanEverTick = true;

}

void UCompensacionLagComponente::BeginPlay() {
	Super::BeginPlay();
	
}

void UCompensacionLagComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

