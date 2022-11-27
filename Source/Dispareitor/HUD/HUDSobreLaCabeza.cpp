#include "HUDSobreLaCabeza.h"
#include "../Personaje/DispareitorPersonaje.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"

// El BP_HUDSobreLaCabeza (que hereda de este) esta incluido en el BP_DispareitorPersonaje y se inicia en el nodo BeginPlay

void UHUDSobreLaCabeza::NativeDestruct() {
    RemoveFromParent();
    Super::NativeDestruct();
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
    if(!bActualizadoTextoSobreLaCabeza) {
         ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(Peon);
    //if(!DispareitorPersonaje->IsLocallyControlled()) {
        ADispareitorEstadoJugador* DispareitorEstadoJugador = Cast<ADispareitorEstadoJugador>(Peon->GetPlayerState());
        if(DispareitorEstadoJugador) {
            FString Nombre = DispareitorEstadoJugador->GetPlayerName();
            UE_LOG(LogTemp, Warning, TEXT("Nombre: %s"), *Nombre);
            ActualizarTextoSobreCabeza(Nombre);
        }
    //}
    }  
}

void UHUDSobreLaCabeza::ActualizarTextoSobreCabeza(FString texto) {
    if(TextoSobreCabeza) {
        TextoSobreCabeza->SetText(FText::FromString(texto));
        bActualizadoTextoSobreLaCabeza = true;
    }
}