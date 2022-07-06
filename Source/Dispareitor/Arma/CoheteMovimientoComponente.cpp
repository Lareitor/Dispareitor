#include "CoheteMovimientoComponente.h"

UCoheteMovimientoComponente::EHandleBlockingHitResult UCoheteMovimientoComponente::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) {
    Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
    // Como tenemos el problema de que a veces el cohete choca con el propio caracter y para su movimiento, devolvemos AdvanceNextSubstep para que el cohete continue con su marcha
    return EHandleBlockingHitResult::AdvanceNextSubstep; 
}

void UCoheteMovimientoComponente::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) {
    // HandleBlockingHit llama por defecto a esta funcion, pero como no queremos que haga nada, la dejamos vacia
}
