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

int xPlayerThrowPos = 0;
int xPlayerDashPos = 0;

int xPlayerShootCooldown = 0;

int xPlayerWeaponDatabase = 0;
int xPlayerWeaponCurrent = 0;

int xPlayerProjSpawner = 0;
int xPlayerSpotlight = 0;
int xPlayerDeflector = 0;
int xPlayerCamo = 0;

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

// weapons
int xWeaponType = 0;
int xWeaponCount = 0;

// units
int dUnits = 0;
int xUnitName = 0;
int xUnitID = 0;
int xUnitOwner = 0;
int xUnitPos = 0;

// projectiles
int xProjDir = 0;
int xProjPrev = 0;
int xProjRadius = 0;

int dKnives = 0;

int initGenericProj(string name = "", float radius = 0) {
	int db = xInitDatabase(name);
	xInitAddInt(db, "name");
	xInitAddInt(db, "id");
	xInitAddInt(db, "owner");
	xInitAddVector(db, "pos");
	xProjRadius = xInitAddFloat(db, "radius", radius);
	xProjDir = xInitAddVector(db, "dir");
	xProjPrev = xInitAddVector(db, "prev");
	return(db);
}

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
	// xPlayerGrapplingHook = xInitAddInt(dPlayerData, "grapplingHook"); // you can have more than one grappling hook
	xPlayerDeflector = xInitAddInt(dPlayerData, "deflector");
	xPlayerCamo = xInitAddInt(dPlayerData, "camo");
	
	xPlayerThrowPos = xInitAddVector(dPlayerData, "throwPos");
	xPlayerDashPos = xInitAddVector(dPlayerData, "dashPos");

	xPlayerWeaponDatabase = xInitAddInt(dPlayerData, "weaponDatabase");
	xPlayerWeaponCurrent = xInitAddInt(dPlayerData, "weaponCurrent");

	xPlayerShootCooldown = xInitAddInt(dPlayerData, "shootCooldown");

	dSpyRequests = xInitDatabase("spyRequests", 32);
	xSpyRequestProto = xInitAddInt(dSpyRequests, "proto");
	xSpyRequestAnim = xInitAddInt(dSpyRequests, "anim");
	xSpyRequestScale = xInitAddVector(dSpyRequests, "scale");
	xSpyRequestDest = xInitAddVector(dSpyRequests, "dest");
	xSpyRequestEvent = xInitAddInt(dSpyRequests, "event");

	dUnits = xInitDatabase("units");
	xUnitName = xInitAddInt(dUnits, "name");
	xUnitID = xInitAddInt(dUnits, "id");
	xUnitOwner = xInitAddInt(dUnits, "owner");
	xUnitPos = xInitAddVector(dUnits, "pos");

	dKnives = initGenericProj("knives", 2);

	int db = 0;
	for(p=1; < cNumberPlayers) {
		xAddDatabaseBlock(dPlayerData, true);
		// LOS frontier
		db = xInitDatabase("losFrontier"+p, 50);
		xFrontierX = xInitAddInt(db, "x");
		xFrontierY = xInitAddInt(db, "y");
		xSetInt(dPlayerData, xPlayerLosFrontier, db);
		// Weapon Database
		db = xInitDatabase("weapons"+p, 3);
		xWeaponType = xInitAddInt(db, "type");
		xWeaponCount = xInitAddInt(db, "count");
		xSetInt(dPlayerData, xPlayerWeaponDatabase, db);
	}
	debugLog("InitDB done");
}