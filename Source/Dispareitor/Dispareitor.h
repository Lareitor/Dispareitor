// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Cuando creamos el canal MallaDelEsqueleto en las propiedades del proyecto le asigna por debajo ECC_GameTraceChannel1 (si crearamos otro le asignaría ECC_GameTraceChannel2 y asi en adelante hasta 18), podemos utilizar este en nuestro proyecto, pero como no es muy descritivo le definimos este alias
#define ECC_MallaDelEsqueleto ECollisionChannel::ECC_GameTraceChannel1
#define ECC_CajaColision ECollisionChannel::ECC_GameTraceChannel2


