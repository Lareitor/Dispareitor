#include "HUDSobreLaCabeza.h"
#include "../Personaje/DispareitorPersonaje.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

// El BP_HUDSobreLaCabeza (que hereda de este) esta incluido en el BP_DispareitorPersonaje y se inicia en el nodo BeginPlay

void UHUDSobreLaCabeza::OnLevelRemovedFromWorld(ULevel *Nivel, UWorld* Mundo) {
    RemoveFromParent();
    Super::OnLevelRemovedFromWorld(Nivel, Mundo);
}	

void UHUDSobreLaCabeza::MostrarJugadorRolRed(APawn* Peon) {
    ENetRole RolLocal = Peon->GetLocalRole();
    FString Rol;   
    
    switch(RolLocal) {
        case ENetRole::ROLE_Authority:
            Rol = FString("Autoridad");
            break;
        case ENetRole::ROLE_AutonomousProxy:
            Rol = FString("Proxy Autonomo");
            break;
        case ENetRole::ROLE_SimulatedProxy:
            Rol = FString("Proxy Simulado");
            break;
        case ENetRole::ROLE_None:
            Rol = FString("Ninguno");
            break;            
    }  

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(Peon);
    APlayerState* EstadoJugador = DispareitorPersonaje->GetPlayerState();
    if(EstadoJugador) {
        Rol = EstadoJugador->GetPlayerName();
    }

    FString RolLocalS = FString::Printf(TEXT("Rol local: %s"), *Rol);
    
    ActualizarTextoSobreCabeza(Rol);
}

// Se invoca en el nodo BP_DispareitorPersonaje::BeginPlay
void UHUDSobreLaCabeza::MostrarJugadorNombre(APawn* Peon) {
    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(Peon);
    if(!DispareitorPersonaje->IsLocallyControlled()) {
        FString Nombre = Peon->GetName();
        ActualizarTextoSobreCabeza(Nombre);
    }
}


void UHUDSobreLaCabeza::ActualizarTextoSobreCabeza(FString texto) {
    if(TextoSobreCabeza) {
        TextoSobreCabeza->SetText(FText::FromString(texto));
    }
}