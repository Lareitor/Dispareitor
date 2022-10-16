#include "AnunciosEliminacion.h"
#include "Components/TextBlock.h"

void UAnunciosEliminacion::ActualizarTextoEliminacion(FString NombreGanador, FString NombrePerdedor) {
    FString TextoEliminacion; 
    if(NombreGanador != NombrePerdedor) {
        TextoEliminacion = FString::Printf(TEXT("%s --> %s"), *NombreGanador, *NombrePerdedor);
    } else {
        TextoEliminacion = FString::Printf(TEXT("--> %s"), *NombrePerdedor);
    }
    if(TextoAnunciosEliminacion) {
        TextoAnunciosEliminacion->SetText(FText::FromString(TextoEliminacion));
    }
}

