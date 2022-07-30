#include "BuffComponente.h"

UBuffComponente::UBuffComponente() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponente::BeginPlay() {
	Super::BeginPlay();
	
}


void UBuffComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

