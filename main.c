rmSetStatusText("", 0.01);

int size = 2*sqrt(MAP_SIZE_STATIC + cNumberNonGaiaPlayers * MAP_SIZE_DYNAMIC);

rmSetMapSize(size, size);

rmTerrainInitialize("OlympusA",1);
rmSetGaiaCiv(cCivGaia);
rmSetLightingSet("Dusk");
rmSetStatusText("", 0.1);