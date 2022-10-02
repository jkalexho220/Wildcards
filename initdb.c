int dPlayerData = 0;
int xPlayerPos = 0;
int xPlayerUnitName = 0;
int xPlayerUnitID = 0;
int xPlayerAlive = 0;
int xPlayerLosStart = 0;
int xPlayerLosDB = 0;
int xPlayerLosFrontier = 0;

// los frontier
int xFrontierX = 0;
int xFrontierY = 0;

rule initDB
active
highFrequency
{
	xsDisableSelf();
	dPlayerData = xInitDatabase("PlayerData", cNumberPlayers - 1);
	xPlayerUnitName = xInitAddInt(dPlayerData, "name");
	xPlayerUnitID = xInitAddInt(dPlayerData, "id");
	xPlayerPos = xInitAddVector(dPlayerData, "pos");
	xPlayerAlive = xInitAddBool(dPlayerData, "alive", false);
	xPlayerLosStart = xInitAddInt(dPlayerData, "losStart");
	xPlayerLosDB = xInitAddInt(dPlayerData, "losDB");
	xPlayerLosFrontier = xInitAddInt(dPlayerData, "losFrontier");

	int db = 0;
	for(p=1; < cNumberPlayers) {
		db = xInitDatabase("losFrontier"+p, 50);
		xFrontierX = xInitAddInt(db, "x");
		xFrontierY = xInitAddInt(db, "y");
		xAddDatabaseBlock(dPlayerData, true);
		xSetInt(dPlayerData, xPlayerLosFrontier, db);
	}
}