# pragma once

UENUM(BlueprintType)
enum class EEquipo  : uint8 {
    EE_Rojo UMETA(DisplayName = "Equipo Rojo"),
    EE_Azul UMETA(DisplayName = "Equipo Azul"),
    EE_Ninguno UMETA(DisplayName = "Ninguno"),    
    EE_Maximo UMETA(DisplayName = "Maximo")
};