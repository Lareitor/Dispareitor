#pragma once

#define RAYO_LONGITUD 80000.f

// Son colores del material PP_Highlight
#define PROFUNDIDAD_PERSONALIZADA_AL_RENDERIZAR_MORADO 250
#define PROFUNDIDAD_PERSONALIZADA_AL_RENDERIZAR_AZUL 251
#define PROFUNDIDAD_PERSONALIZADA_AL_RENDERIZAR_BLAQUECINO 252


UENUM(BlueprintType)
enum class ETipoArma : uint8 {
    ETA_RifleAsalto UMETA(DisplayName = "Rifle Asalto"),
    ETA_LanzaCohetes UMETA(DisplayName = "LanzaCohetes"),
    ETA_Pistola UMETA(DisplayName = "Pistola"),
    ETA_Subfusil UMETA(DisplayName = "Subfusil"),
    ETA_Escopeta UMETA(DisplayName = "Escopeta"),
    ETA_Francotirador UMETA(DisplayName = "Francotirador"),
    ETA_LanzaGranadas UMETA(DisplayName = "LanzaGranadas"),
    ETA_Bandera UMETA(DisplayName = "Bandera"),
    ETA_Maximo UMETA(DisplayName = "Maximo")
};