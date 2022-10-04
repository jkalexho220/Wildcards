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

int xPlayerDashCount = 0;
int xPlayerDashCooldown = 0;
int xPlayerDashStep = 0;

int xPlayerPoints = 0;
int xPlayerRespawnTime = 0;

int xPlayerShootCooldown = 0;

int xPlayerWeaponDatabase = 0;
int xPlayerWeaponCurrent = 0;
int xPlayerWeaponTop = 0;

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

// collectibles
int dCollectibles = 0;
int xCollectiblePad = 0;
int xCollectibleObject = 0;
int xCollectibleType = 0;
int xCollectibleCount = 0;

int dGoldMines = 0;
int xGoldMineObject = 0;
int xGoldMineFlag = 0;
int xGoldMineRevealer = 0;
int xGoldMineHolder = 0;
int xGoldMineCircle = 0;
int xGoldMineValue = 0;

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

	xPlayerProto = xInitAddString(dPlayerData, "proto", "Peltast");

	xPlayerPoints = xInitAddInt(dPlayerData, "points", 0);
	xPlayerRespawnTime = xInitAddInt(dPlayerData, "respawnTime");

	xPlayerProjSpawner = xInitAddInt(dPlayerData, "projSpawner");
	xPlayerSpotlight = xInitAddInt(dPlayerData, "spotlight");
	// xPlayerGrapplingHook = xInitAddInt(dPlayerData, "grapplingHook"); // you can have more than one grappling hook
	xPlayerDeflector = xInitAddInt(dPlayerData, "deflector");
	xPlayerCamo = xInitAddInt(dPlayerData, "camo");
	
	xPlayerThrowPos = xInitAddVector(dPlayerData, "throwPos");
	xPlayerDashPos = xInitAddVector(dPlayerData, "dashPos");

	xPlayerWeaponDatabase = xInitAddInt(dPlayerData, "weaponDatabase");
	xPlayerWeaponTop = xInitAddInt(dPlayerData, "weaponTop");
	xPlayerWeaponCurrent = xInitAddInt(dPlayerData, "weaponCurrent");

	xPlayerShootCooldown = xInitAddInt(dPlayerData, "shootCooldown");

	xPlayerDashCount = xInitAddInt(dPlayerData, "dashCount", 2);
	xPlayerDashCooldown = xInitAddInt(dPlayerData, "dashCooldown");
	xPlayerDashStep = xInitAddInt(dPlayerData, "dashStep");

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

	dKnives = initGenericProj("knives", 2.0);

	dCollectibles = xInitDatabase("collectibles");
	xCollectibleObject = xInitAddInt(dCollectibles, "object");
	xCollectiblePad = xInitAddInt(dCollectibles, "pad");
	xCollectibleType = xInitAddInt(dCollectibles, "type");
	xInitAddVector(dCollectibles, "pos");
	xCollectibleCount = xInitAddInt(dCollectibles, "count", 1);

	dGoldMines = xInitDatabase("goldMines");
	xGoldMineObject = xInitAddInt(dGoldMines, "object");
	xGoldMineFlag = xInitAddInt(dGoldMines, "flag");
	xGoldMineHolder = xInitAddInt(dGoldMines, "holder");
	xInitAddVector(dGoldMines, "pos");
	xGoldMineRevealer = xInitAddInt(dGoldMines, "revealer");
	xGoldMineCircle = xInitAddInt(dGoldMines, "circle");
	xGoldMineValue = xInitAddInt(dGoldMines, "value");

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