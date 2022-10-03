int dPlayerData = 0;
int xPlayerPos = 0;
int xPlayerUnitName = 0;
int xPlayerUnitID = 0;
int xPlayerAlive = 0;
int xPlayerLosStart = 0;
int xPlayerLosDB = 0;
int xPlayerLosFrontier = 0;

int xPlayerCharacter = 0;
int xPlayerPro = 0;


int xPlayerProjSpawner = 0;
int xPlayerSpotlight = 0;

int xPlayerProto = 0;

// los frontier
int xFrontierX = 0;
int xFrontierY = 0;

int dSpyRequests = 0;
int xSpyRequestProto = 0;
int xSpyRequestDest = 0;
int xSpyRequestScale = 0;
int xSpyRequestAnim = 0;
int xSpyRequestEvent = 0;

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

	xPlayerPro = xInitAddInt(dPlayerData, "pro");
	xPlayerCharacter = xInitAddInt(dPlayerData, "char");

	xPlayerProto = xInitAddString(dPlayerData, "proto", "Hoplite");

	xPlayerProjSpawner = xInitAddInt(dPlayerData, "projSpawner");
	xPlayerSpotlight = xInitAddInt(dPlayerData, "spotlight");

	dSpyRequests = xInitDatabase("spyRequests", 32);
	xSpyRequestProto = xInitAddInt(dSpyRequests, "proto");
	xSpyRequestAnim = xInitAddInt(dSpyRequests, "anim");
	xSpyRequestScale = xInitAddVector(dSpyRequests, "scale");
	xSpyRequestDest = xInitAddVector(dSpyRequests, "dest");
	xSpyRequestEvent = xInitAddInt(dSpyRequests, "event");

	int db = 0;
	for(p=1; < cNumberPlayers) {
		db = xInitDatabase("losFrontier"+p, 50);
		xFrontierX = xInitAddInt(db, "x");
		xFrontierY = xInitAddInt(db, "y");
		xAddDatabaseBlock(dPlayerData, true);
		xSetInt(dPlayerData, xPlayerLosFrontier, db);
	}
	debugLog("InitDB done");
}