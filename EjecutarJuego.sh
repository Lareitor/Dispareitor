#!/bin/bash

export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1

cd /home/lareitor/Programas/UnrealEngine-release/Engine/Binaries/Linux
./UnrealEditor /home/lareitor/Proyectos/Unreal/Dispareitor/Dispareitor/Dispareitor.uproject -game
