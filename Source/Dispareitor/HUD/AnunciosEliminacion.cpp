#include "AnunciosEliminacion.h"
#include "Components/TextBlock.h"

void UAnunciosEliminacion::ActualizarTextoEliminacion(FString NombreGanador, FString NombrePerdedor) {
    FString TextoEliminacion = FString::Printf(TEXT("%s --> %s"), *NombreGanador, *NombrePerdedor);
    if(TextoAnunciosEliminacion) {
        TextoAnunciosEliminacion->SetText(FText::FromString(TextoEliminacion));
    }
}

