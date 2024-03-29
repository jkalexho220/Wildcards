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

int xPlayerIndex = 0;

int xPlayerProjSpawner = 0;
int xPlayerSpotlight = 0;
int xPlayerDeflector = 0;
int xPlayerWhirlwindActive = 0;
int xPlayerWhirlwindTimeout = 0;

int xPlayerCamo = 0;

int xPlayerProto = 0;

int xPlayerSnared = 0;
int xPlayerSnareTime = 0;
int xPlayerSpeed = 0;

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
int xUnitLaunched = 0;
int xUnitStationary = 0;

// projectiles
int xProjDir = 0;
int xProjPrev = 0;
int xProjRadius = 0;
int xProjTimeout = 0;

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

int dGrapplingHooks = 0;
int xGrapplingHookLineProj = 0;
int xGrapplingHookLinePlayer = 0;
int xGrapplingHookStep = 0;
int xGrapplingHookIndex = 0;
int xGrapplingHookTarget = 0;

int dPortalShots = 0;

int dPortals = 0;
int xPortalUnitsStart = 0;
int xPortalUnitsEnd = 0;
int xPortalStart = 0;
int xPortalEnd = 0;
int xPortalTimeout = 0;

int dLaunchedUnits = 0;
int xLaunchedCar = 0;
int xLaunchedDB = 0;
int xLaunchedIndex = 0;
int xLaunchedDest = 0;
int xLaunchedTimeout = 0;
int xLaunchedProto = 0;

int dTraps = 0;
int xTrapArmTime = 0;
int xTrapArmed = 0;

int smokeArray = 0;
int dSmokeTiles = 0;
int xSmokeTileX = 0;
int xSmokeTileY = 0;
int xSmokeTileTerrainType = 0;
int xSmokeTileTerrainSub = 0;

int dSmokeBombs = 0;
int xSmokeBombDest = 0;

int frostArray = 0;
int dFrostCrates = 0;
int dExplosiveCrates = 0;

int xCrateActive = 0;
int xCrateTimeout = 0;
int xCrateUnitsEnd = 0;

int dPellets = 0;

int initGenericUnit(string name = "") {
	int db = xInitDatabase(name);
	xInitAddInt(db, "name");
	xInitAddInt(db, "id");
	xInitAddInt(db, "owner", 0);
	xInitAddVector(db, "pos");
	return(db);
}

int initGenericProj(string name = "", float radius = 0) {
	int db = xInitDatabase(name);
	xInitAddInt(db, "name");
	xInitAddInt(db, "id");
	xInitAddInt(db, "owner");
	xInitAddVector(db, "pos");
	xProjRadius = xInitAddFloat(db, "radius", radius);
	xProjDir = xInitAddVector(db, "dir");
	xProjPrev = xInitAddVector(db, "prev");
	xProjTimeout = xInitAddInt(db, "timeout");
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
	xPlayerIndex = xInitAddInt(dPlayerData, "index");

	xPlayerProjSpawner = xInitAddInt(dPlayerData, "projSpawner");
	xPlayerSpotlight = xInitAddInt(dPlayerData, "spotlight");
	// xPlayerGrapplingHook = xInitAddInt(dPlayerData, "grapplingHook"); // you can have more than one grappling hook
	xPlayerDeflector = xInitAddInt(dPlayerData, "deflector");
	xPlayerWhirlwindActive = xInitAddBool(dPlayerData, "whirlwindActive");
	xPlayerWhirlwindTimeout = xInitAddInt(dPlayerData, "whirlwindTimeout");

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

	xPlayerSnared = xInitAddBool(dPlayerData, "snared", false);
	xPlayerSnareTime = xInitAddInt(dPlayerData, "snareTime");
	xPlayerSpeed = xInitAddFloat(dPlayerData, "speed", 5.0);

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
	xUnitLaunched = xInitAddBool(dUnits, "launched", false);
	xUnitStationary = xInitAddBool(dUnits, "stationary", false);

	dKnives = initGenericProj("knives", 2.0);

	dGrapplingHooks = initGenericProj("grapplingHooks", 4.0);
	xGrapplingHookLineProj = xInitAddInt(dGrapplingHooks, "lineProj");
	xGrapplingHookLinePlayer = xInitAddInt(dGrapplingHooks, "linePlayer");
	xGrapplingHookStep = xInitAddInt(dGrapplingHooks, "step", 0);
	xGrapplingHookIndex = xInitAddInt(dGrapplingHooks, "index");
	xGrapplingHookTarget = xInitAddInt(dGrapplingHooks, "target");

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

	dLaunchedUnits = xInitDatabase("launchedUnits",10);
	xInitAddInt(dLaunchedUnits,"name");
	xInitAddInt(dLaunchedUnits,"id");
	xInitAddInt(dLaunchedUnits,"owner");
	xLaunchedCar = xInitAddInt(dLaunchedUnits,"car");
	xLaunchedDB = xInitAddInt(dLaunchedUnits,"database");
	xLaunchedIndex = xInitAddInt(dLaunchedUnits,"index");
	xLaunchedDest = xInitAddVector(dLaunchedUnits,"dest");
	xLaunchedTimeout = xInitAddInt(dLaunchedUnits,"timeout");
	xLaunchedProto = xInitAddInt(dLaunchedUnits, "proto");

	dPortalShots = initGenericProj("portals", 3.0);

	dPortals = xInitDatabase("portals");
	xPortalUnitsStart = xInitAddInt(dPortals, "unit1");
	xPortalUnitsEnd = xInitAddInt(dPortals, "unit2");
	xPortalStart = xInitAddVector(dPortals, "start");
	xPortalEnd = xInitAddVector(dPortals, "end");
	xPortalTimeout = xInitAddInt(dPortals, "timeout");

	dTraps = initGenericUnit("traps");
	xTrapArmTime = xInitAddInt(dTraps, "armTime");
	xTrapArmed = xInitAddBool(dTraps, "armed", false);

	dSmokeTiles = xInitDatabase("smoke");
	xSmokeTileX = xInitAddInt(dSmokeTiles, "x");
	xSmokeTileY = xInitAddInt(dSmokeTiles, "y");
	xSmokeTileTerrainType = xInitAddInt(dSmokeTiles, "terrainType");
	xSmokeTileTerrainSub = xInitAddInt(dSmokeTiles, "terrainSubType");

	dSmokeBombs = initGenericProj("smokeBombs", 2.0);
	xSmokeBombDest = xInitAddVector(dSmokeBombs, "destination");

	dFrostCrates = initGenericUnit("frostCrates");
	xCrateUnitsEnd = xInitAddInt(dFrostCrates, "units");
	xCrateActive = xInitAddBool(dFrostCrates, "active", false);
	xCrateTimeout = xInitAddInt(dFrostCrates, "timeout");
	dExplosiveCrates = initGenericUnit("explosiveCrates");
	xInitAddInt(dExplosiveCrates, "units");

	dPellets = initGenericProj("pellets", 2.0);

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