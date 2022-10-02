

void spawnPlayer(int p = 0, vector pos = vector(0,0,0)) {
	int old = xGetPointer(dPlayerData);
	xSetPointer(dPlayerData, p);
	xSetInt(dPlayerData, xPlayerUnitName, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+p+",0", "Hoplite", 1, xsVectorGetX(pos), 0, xsVectorGetZ(pos), 0, true);
	xSetInt(dPlayerData, xPlayerUnitID, kbGetBlockID(""+xGetInt(dPlayerData, xPlayerUnitName), true));
	xSetBool(dPlayerData, xPlayerAlive, true);
	xSetVector(dPlayerData, xPlayerPos, pos);
	int x = xsVectorGetX(pos) / 4;
	int y = xsVectorGetZ(pos) / 4;
	addFrontier(x * 2, y * 2);
	cleanFrontier();
	xSetPointer(dPlayerData, old);
}