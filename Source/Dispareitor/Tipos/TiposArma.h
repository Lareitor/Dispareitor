#pragma once

UENUM(BlueprintType)
enum class ETipoArma : uint8 {
    ETA_RifleAsalto UMETA(DisplayName = "Rifle Asalto"),
    ETA_LanzaCohetes UMETA(DisplayName = "LanzaCohetes"),
    ETA_Maximo UMETA(DisplayName = "Maximo")
};