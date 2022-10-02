rmSetStatusText("", 0.01);

int size = 2*sqrt(cNumberNonGaiaPlayers * 2000);

rmSetMapSize(size, size);

rmTerrainInitialize("OlympusA",1);
rmSetGaiaCiv(cCivGaia);
rmSetLightingSet("Dusk");
rmSetStatusText("", 0.1);