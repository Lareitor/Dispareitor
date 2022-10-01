# pragma once

UENUM(BlueprintType)
enum class EEstadosCombate  : uint8 {
    EEC_Desocupado UMETA(DisplayName = "Desocupado"),
    EEC_Recargando UMETA(DisplayName = "Recargando"),
    EEC_LanzandoGranada UMETA(DisplayName = "Lanzando granada"),
    EEC_IntercambiandoArmas UMETA(DisplayName = "Intercambiando armas"),
    EEC_Maximo UMETA(DisplayName = "Maximo")
};