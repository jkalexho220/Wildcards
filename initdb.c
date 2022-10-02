int dPlayerData = 0;
int xPlayerUnitName = 0;
int xPlayerUnitID = 0;
int xPlayerLosStart = 0;
int xPlayerLosDB = 0;

rule initDB
active
highFrequency
{
	xsDisableSelf();
	dPlayerData = xInitDatabase("PlayerData", cNumberPlayers - 1);
	xPlayerUnitName = xInitAddInt(dPlayerData, "name");
	xPlayerUnitID = xInitAddInt(dPlayerData, "id");
	xPlayerLosStart = xInitAddInt(dPlayerData, "losStart");
	xPlayerLosDB = xInitAddInt(dPlayerData, "losDB");
	
	for(p=1; < cNumberPlayers) {
		xAddDatabaseBlock(dPlayerData);
	}
}