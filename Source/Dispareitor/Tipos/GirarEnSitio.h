# pragma once

UENUM(BlueprintType)
enum class EGirarEnSitio  : uint8 {
    EGES_Izquierda UMETA(DisplayName = "Girar izquierda"),
    EGES_Derecha UMETA(DisplayName = "Girar derecha"),
    EGES_NoGirar UMETA(DisplayName = "No girar"),
    EGES_Maximo UMETA(DisplayName = "Maximo")
};