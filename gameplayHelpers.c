int spyreset = 0;
int spysearch = 0;
int wildcard = -1;
int wildcardStep = 0;
int wildcardNext = 0;

int knifeCount = 0;
int nextCollectible = 0;
int nextCrate = 0;

bool inPortal = false;

void reselectMyself() {
	uiClearSelection();
	int p = trCurrentPlayer();
	string proto = xGetString(dPlayerData, xPlayerProto, p);
	trackInsert();
	trackAddWaypoint();
	trackAddWaypoint();
	trBlockAllSounds(false);
	uiFindType(proto);
	trackPlay(1,EVENT_REMOVE_CAM_TRACKS);
}

void overlayTextColor(int p = 0) {
	switch(p)
	{
		case 1:
		{
			trOverlayTextColour(21, 21, 200);
		}
		case 2:
		{
			trOverlayTextColour(216, 21, 20);
		}
		case 3:
		{
			trOverlayTextColour(0, 125, 0);
		}
		case 4:
		{
			trOverlayTextColour(42, 195, 200);
		}
		case 5:
		{
			trOverlayTextColour(189, 43, 187);
		}
		case 6:
		{
			trOverlayTextColour(169, 166, 0);
		}
		case 7:
		{
			trOverlayTextColour(216, 50, 0);
		}
		case 8:
		{
			trOverlayTextColour(108, 50, 0);
		}
		case 9:
		{
			trOverlayTextColour(42, 212, 39);
		}
		case 10:
		{
			trOverlayTextColour(152, 209, 146);
		}
		case 11:
		{
			trOverlayTextColour(68, 66, 63);
		}
		case 12:
		{
			trOverlayTextColour(216, 0, 80);
		}
	}
}

void displayWildcardDetails() {
	trOverlayText(trStringQuestVarGet("p"+wildcard+"name") + " - " + xGetInt(dPlayerData, xPlayerPoints, wildcard), 9999, 10, 12, 1600);
}

void earnPoints(int p = 0, int val = 0) {
	xSetInt(dPlayerData, xPlayerPoints, xGetInt(dPlayerData, xPlayerPoints, p) + val, p);
}


bool targetEligible(int p = 0) {
	return((p == wildcard) || (xGetInt(dUnits, xUnitOwner) == wildcard) || (xGetInt(dUnits, xUnitOwner) == 0));
}

/*
Assumes that the target unit is already selected
*/
void spyEffect(int proto = 0, int anim = 0, vector dest = vector(0,0,0), vector scale = vector(1,1,1), int event = -1) {
	int newest = xAddDatabaseBlock(dSpyRequests);
	xSetInt(dSpyRequests, xSpyRequestProto, proto, newest);
	xSetInt(dSpyRequests, xSpyRequestAnim, anim, newest);
	xSetVector(dSpyRequests, xSpyRequestDest, dest, newest);
	xSetVector(dSpyRequests, xSpyRequestScale, scale, newest);
	xSetInt(dSpyRequests, xSpyRequestEvent, event, newest);
	trTechInvokeGodPower(0, "spy", vector(0,0,0), vector(0,0,0));
}

bool rayCollision(int db = 0, vector start = vector(0,0,0), vector dir = vector(0,0,0), float dist = 0, float width = 0) {
	vector pos = xGetVector(db, xUnitPos);
	float current = distanceBetweenVectors(pos, start, false);
	if (current < dist) {
		vector hitbox = xsVectorSet(xsVectorGetX(start) + current * xsVectorGetX(dir),0,
			xsVectorGetZ(start) + current * xsVectorGetZ(dir));
		if (distanceBetweenVectors(pos, hitbox, true) <= width) {
			return(true);
		}
	}
	return(false);
}

int addUnit(int name = 0, int id = 0, int p = 0) {
	xAddDatabaseBlock(dUnits, true);
	xSetInt(dUnits, xUnitName, name);
	xSetInt(dUnits, xUnitID, id);
	xSetInt(dUnits, xUnitOwner, p);
	return(xGetNewestPointer(dUnits));
}

void spawnPlayer(int p = 0, vector pos = vector(0,0,0)) {
	int old = xGetPointer(dPlayerData);
	xSetPointer(dPlayerData, p);

	xSetInt(dPlayerData, xPlayerDashCount, 2);

	xSetInt(dPlayerData, xPlayerUnitName, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+p+",0", xGetString(dPlayerData, xPlayerProto), 1, xsVectorGetX(pos), 0, xsVectorGetZ(pos), 0, true);
	xSetInt(dPlayerData, xPlayerUnitID, kbGetBlockID(""+xGetInt(dPlayerData, xPlayerUnitName), true));
	xSetBool(dPlayerData, xPlayerAlive, true);
	xSetVector(dPlayerData, xPlayerPos, pos);

	xSetInt(dPlayerData, xPlayerIndex, addUnit(xGetInt(dPlayerData, xPlayerUnitName), xGetInt(dPlayerData, xPlayerUnitID), p));
	int x = xsVectorGetX(pos) / 4;
	int y = xsVectorGetZ(pos) / 4;

	addFrontier(x * 2, y * 2);
	cleanFrontier();

	debugLog(xGetString(dPlayerData, xPlayerProto) + ":" + xGetInt(dPlayerData, xPlayerUnitName) + ":" + trGetNextUnitScenarioNameNumber());

	xUnitSelectByID(dPlayerData, xPlayerUnitID);
	spyEffect(kbGetProtoUnitID("Cinematic Block"), 0, xsVectorSet(dPlayerData, xPlayerProjSpawner, p));
	spyEffect(kbGetProtoUnitID("Cinematic Block"), 0, xsVectorSet(dPlayerData, xPlayerDeflector, p));
	xSetPointer(dPlayerData, old);
}

vector vectorSetAsTargetVector(vector from = vector(0,0,0), vector dir = vector(0,0,0), float dist = 40.0) {
	vector target = xsVectorSet((xsVectorGetX(dir) * dist) + xsVectorGetX(from), 0, (xsVectorGetZ(dir) * dist) + xsVectorGetZ(from));
	
	/*
	No out-of-bounds allowed
	*/
	float scale = 0;
	if (xsVectorGetX(target) < 0) {
		scale = xsVectorGetX(target) / (xsVectorGetX(target) - xsVectorGetX(from));
		target = xsVectorSet(0,0, xsVectorGetZ(target) + scale * (xsVectorGetZ(from) - xsVectorGetZ(target)));
	} else if (xsVectorGetX(target) > (mapSize * 2 + 1)) {
		scale = (xsVectorGetX(target) - (mapSize * 2 + 1)) / (xsVectorGetX(target) - xsVectorGetX(from));
		target = xsVectorSet(mapSize * 2 + 1,0,xsVectorGetZ(target) + scale * (xsVectorGetZ(from) - xsVectorGetZ(target)));
	}
	
	if (xsVectorGetZ(target) < 0) {
		scale = xsVectorGetZ(target) / (xsVectorGetZ(target) - xsVectorGetZ(from));
		target = xsVectorSet(xsVectorGetX(target) + scale * (xsVectorGetX(from) - xsVectorGetX(target)),0,0);
	} else if (xsVectorGetZ(target) > (mapSize * 2 + 1)) {
		scale = (xsVectorGetZ(target) - (mapSize * 2 + 1)) / (xsVectorGetZ(target) - xsVectorGetZ(from));
		target = xsVectorSet(xsVectorGetX(target) + scale * (xsVectorGetX(from) - xsVectorGetX(target)),0,mapSize * 2 + 1);
	}
	return(target);
}

int launchUnit(int db = 0, vector dest = vector(0,0,0), bool ignoreWalls = false) {
	int index = -1;
	if (xGetBool(db, xUnitLaunched) == false) {
		xSetBool(db, xUnitLaunched, true);
		int type = kbGetUnitBaseTypeID(xGetInt(db, xUnitID));
		int p = xGetInt(db, xUnitOwner);

		xUnitSelectByID(db,xUnitID);
		trUnitChangeProtoUnit("Transport Ship Greek");
		
		vector start = kbGetBlockPosition(""+xGetInt(db,xUnitName));
		vector dir = getUnitVector(start,dest);
		
		int next = trGetNextUnitScenarioNameNumber();
		trArmyDispatch(""+p+",0","Dwarf",1,1,0,1,0,true);
		trUnitSelectClear();
		trUnitSelect(""+next,true);
		trImmediateUnitGarrison(""+xGetInt(db,xUnitName));
		trUnitChangeProtoUnit("Dwarf");
		
		trUnitSelectClear();
		trUnitSelect(""+next,true);
		trSetUnitOrientation(dir, vector(0,1,0), true);
		trMutateSelected(kbGetProtoUnitID("Lancer Hero"));
		
		xUnitSelectByID(db,xUnitID);
		trMutateSelected(type);
		trUnitOverrideAnimation(24,0,true,true,-1);
		trMutateSelected(kbGetProtoUnitID("Relic"));
		trImmediateUnitGarrison(""+next);
		trMutateSelected(type);
		
		float dist = distanceBetweenVectors(start, dest, false);
		for(x=0; < dist / 2) {
			vector nextpos = xsVectorSet(xsVectorGetX(start) + 2.0 * xsVectorGetX(dir),0,
				xsVectorGetZ(start) + 2.0 * xsVectorGetZ(dir));
			if (terrainIsType(vectorToGrid(nextpos), TERRAIN_WALL, TERRAIN_WALL_SUB) && (ignoreWalls == false)) {
				break;
			} else {
				start = nextpos;
			}
		}
		
		trUnitSelectClear();
		trUnitSelect(""+next,true);
		trMutateSelected(kbGetProtoUnitID("Wadjet Spit"));
		trUnitMoveToPoint(xsVectorGetX(start),0,xsVectorGetZ(start),-1,false);
		
		index = xAddDatabaseBlock(dLaunchedUnits, true);
		xSetInt(dLaunchedUnits,xUnitName,xGetInt(db,xUnitName));
		xSetInt(dLaunchedUnits,xUnitOwner,xGetInt(db,xUnitOwner));
		xSetInt(dLaunchedUnits,xUnitID,xGetInt(db,xUnitID));
		xSetInt(dLaunchedUnits, xLaunchedCar, next);
		xSetInt(dLaunchedUnits,xLaunchedIndex,xGetPointer(db));
		xSetVector(dLaunchedUnits,xLaunchedDest,start);
		xSetInt(dLaunchedUnits,xLaunchedTimeout, trTimeMS() + 1100 * dist / 15);
		xSetInt(dLaunchedUnits, xLaunchedDB, db);
		xSetInt(dLaunchedUnits, xLaunchedProto, type);
	}
	return(index);
}


void processLaunchedUnit() {
	for(i=xGetDatabaseCount(dLaunchedUnits); >0) {
		xDatabaseNext(dLaunchedUnits);
		vector dest = xGetVector(dLaunchedUnits,xLaunchedDest);
		vector pos = kbGetBlockPosition(""+xGetInt(dLaunchedUnits, xUnitName));
		xUnitSelectByID(dLaunchedUnits,xUnitID);
		if (trUnitAlive() == false ||
			distanceBetweenVectors(pos, dest) < 4.0 ||
			trTimeMS() > xGetInt(dLaunchedUnits, xLaunchedTimeout)) {
			if (trUnitAlive()) {
				int p = xGetInt(dLaunchedUnits,xUnitOwner);
				int db = xGetInt(dLaunchedUnits,xLaunchedDB);
				xSetBool(db, xUnitLaunched, false, xGetInt(dLaunchedUnits,xLaunchedIndex));
				trUnitChangeProtoUnit(kbGetProtoUnitName(xGetInt(dLaunchedUnits,xLaunchedProto)));
				xUnitSelect(dLaunchedUnits,xUnitName);
				trMutateSelected(xGetInt(dLaunchedUnits,xLaunchedProto));
			} else {
				trUnitChangeProtoUnit(kbGetProtoUnitName(xGetInt(dLaunchedUnits,xLaunchedProto)));
			}
			xUnitSelect(dLaunchedUnits,xLaunchedCar);
			trUnitChangeProtoUnit("Dust Small");
			xFreeDatabaseBlock(dLaunchedUnits);
		}
	}
}

void paintSmokeTile(int x = 0, int y = 0, int timestamp = 5000) {
	if (timestamp > aiPlanGetUserVariableInt(smokeArray, x, y)) {
		if (aiPlanGetUserVariableInt(smokeArray, x, y) == 0) {
			xAddDatabaseBlock(dSmokeTiles, true);
			xSetInt(dSmokeTiles, xSmokeTileX, x);
			xSetInt(dSmokeTiles, xSmokeTileY, y);
			xSetInt(dSmokeTiles, xSmokeTileTerrainType, trGetTerrainType(x, y));
			xSetInt(dSmokeTiles, xSmokeTileTerrainSub, trGetTerrainSubType(x, y));
			trPaintTerrain(x, y, x, y, 2, 5, false); // cliff plain
		}
		aiPlanSetUserVariableInt(smokeArray, x, y, timestamp);
	}
}

string getCrateProto(int db = 0) {
	string proto = "Ice Block";
	switch(db)
	{
		case dFrostCrates:
		{
			proto = "Ice Block";
		}
		case dExplosiveCrates:
		{
			proto = "Ball of Fire Impact";
		}
	}
	return(proto);
}

vector getCrateScale(int db = 0) {
	vector scale = vector(1,1,1);
	switch(db)
	{
		case dFrostCrates:
		{
			scale = vector(1.5, 1.5, 1.5);
		}
		case dExplosiveCrates:
		{
			scale = vector(1.0, 1.0, 1.0);
		}
	}
	return(scale);
}

void spawnCrate(vector pos = vector(0,0,0), int db = 0, string proto = "Ice Block", vector scale = vector(1.5,1.5,1.5)) {
	xAddDatabaseBlock(dUnits, true);
	xSetInt(dUnits, xUnitName, trGetNextUnitScenarioNameNumber());
	xSetBool(dUnits, xUnitLaunched, true);
	xSetBool(dUnits, xUnitStationary, true);
	xSetVector(dUnits, xUnitPos, vectorSnapToGrid(pos));
	xAddDatabaseBlock(db, true);
	xSetInt(db, xUnitName, trGetNextUnitScenarioNameNumber());
	trArmyDispatch("0,0","Dwarf",3,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
	xSetInt(db, xCrateUnitsEnd, trGetNextUnitScenarioNameNumber());
	xSetInt(db, xUnitID, kbGetBlockID(""+xGetInt(db, xUnitName), true));
	xSetVector(db, xUnitPos, vectorToGrid(pos));
	xUnitSelectByID(db, xUnitID);
	trUnitChangeProtoUnit("Cinematic Block");
	trUnitSelectClear();
	trUnitSelect(""+(1 + xGetInt(db, xUnitName)), true);
	trMutateSelected(kbGetProtoUnitID(proto));
	trUnitOverrideAnimation(2,0,true,true,-1);
	trSetSelectedScale(xsVectorGetX(scale),xsVectorGetY(scale),xsVectorGetZ(scale));
	trUnitSelectClear();
	trUnitSelect(""+(2 + xGetInt(db, xUnitName)), true);
	trUnitChangeProtoUnit("Crate");
	trUnitSelectClear();
	trUnitSelect(""+(2 + xGetInt(db, xUnitName)), true);
	trUnitSetAnimationPath("0,0,0,0,0,0,0");

	xSetInt(dUnits, xUnitID, xGetInt(db, xUnitID));
}

int spawnObject(int p = 0, string proto = "") {
	int next = trGetNextUnitScenarioNameNumber() + 1;
	xSetPointer(dPlayerData, p);
	xUnitSelect(dPlayerData, xPlayerProjSpawner);
	trUnitChangeProtoUnit("Hero Norse");

	if (trGetNextUnitScenarioNameNumber() + 1 == next) {
		debugLog("Missing proj spawner!");
		xUnitSelectByID(dPlayerData, xPlayerUnitID);
		spyEffect(kbGetProtoUnitID("Cinematic Block"), 0, xsVectorSet(dPlayerData, xPlayerProjSpawner, p));
		next = trGetNextUnitScenarioNameNumber();
		vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName), true);
		trArmyDispatch(""+p+",0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
	}

	trUnitSelectClear();
	trUnitSelect(""+next, true);
	trUnitConvert(p);
	trUnitChangeProtoUnit("Dwarf");

	xUnitSelect(dPlayerData, xPlayerProjSpawner);
	trMutateSelected(kbGetProtoUnitID("Cinematic Block"));
	
	trUnitSelectClear();
	trUnitSelect(""+next, true);
	trMutateSelected(kbGetProtoUnitID(proto));
	return(next);
}

void spawnGenericProj(int p = 0, int db = 0, string proto = "", vector pos = vector(0,0,0), vector dir = vector(0,0,0)) {
	xAddDatabaseBlock(db, true);
	xSetInt(db, xUnitName, spawnObject(p, proto));
	xSetInt(db, xUnitID, kbGetBlockID(""+xGetInt(db, xUnitName), true));
	xSetInt(db, xUnitOwner, p);
	xSetVector(db, xProjPrev, pos);
	xSetVector(db, xProjDir, dir);
}

void shootGenericProj(int p = 0, int db = 0, string proto = "", vector dest = vector(0,0,0)) {
	vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName, p), true);
	vector dir = getUnitVector(pos, dest);
	dest = vectorSetAsTargetVector(pos, dir, 999.0);

	spawnGenericProj(p, db, proto, pos, dir);
	
	xUnitSelectByID(db, xUnitID);
	trSetUnitOrientation(dir, vector(0,1,0), true);
	trUnitMoveToPoint(xsVectorGetX(dest),0,xsVectorGetZ(dest),-1,false);
}

void shootGrapplingHook(int p = 0, vector dest = vector(0,0,0)) {
	vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName, p), true);
	vector dir = getUnitVector(pos, dest);
	dest = vectorSetAsTargetVector(pos, dir, 999.0);

	spawnGenericProj(p, dGrapplingHooks, "Lancer Hero", pos, dir);
	xSetInt(dGrapplingHooks, xGrapplingHookLineProj, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+p+",0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
	trArmySelect(""+p+",0");
	trMutateSelected(kbGetProtoUnitID("Torch"));
	trUnitSetAnimationPath("2,0,1,1,1,0,0");

	trMutateSelected(kbGetProtoUnitID("Relic"));
	trImmediateUnitGarrison(""+xGetInt(dGrapplingHooks, xUnitName));
	trMutateSelected(kbGetProtoUnitID("Torch"));
	trSetSelectedUpVector(0,-1,0);

	trUnitSelectClear();
	trUnitSelectByID(xGetInt(dPlayerData, xPlayerUnitID, p));
	spyEffect(kbGetProtoUnitID("Cinematic Block"), -1, xsVectorSet(dGrapplingHooks, xGrapplingHookLinePlayer, xGetNewestPointer(dGrapplingHooks)));

	xUnitSelectByID(dGrapplingHooks, xUnitID);
	trMutateSelected(kbGetProtoUnitID("Wadjet Spit"));
	trSetUnitOrientation(dir, vector(0,1,0), true);
	trUnitMoveToPoint(xsVectorGetX(dest),0,xsVectorGetZ(dest),-1,false);
}

void spawnCollectible(vector pos = vector(0,0,0), int type = 0, int count = 1) {
	if (type == WEAPON_KNIFE) {
		knifeCount = knifeCount + 1;
	}
	xAddDatabaseBlock(dCollectibles, true);
	xSetInt(dCollectibles, xCollectiblePad, trGetNextUnitScenarioNameNumber());
	trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
	xUnitSelect(dCollectibles, xCollectiblePad);
	trUnitChangeProtoUnit("Spy Eye");
	xUnitSelect(dCollectibles, xCollectiblePad);
	trMutateSelected(kbGetProtoUnitID("Outpost"));
	trSetSelectedScale(0,0,0);
	trUnitSetAnimationPath("1,1,0,0,0,0,0");

	xSetInt(dCollectibles, xCollectibleObject, trGetNextUnitScenarioNameNumber());
	trArmyDispatch("0,0", "Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),225,true);
	xUnitSelect(dCollectibles, xCollectibleObject);
	trSetSelectedScale(0.5,0.5,0.5);
	trMutateSelected(weaponProto(type));
	if (type == WEAPON_KNIFE) {
		trSetSelectedScale(1.5, 1.5, 1.5);
		trSetUnitOrientation(vector(0,-1,0),vector(-0.707107,0,-0.707107),true);
	}

	xSetInt(dCollectibles, xCollectibleType, type);
	xSetVector(dCollectibles, xUnitPos, kbGetBlockPosition(""+xGetInt(dCollectibles, xCollectiblePad), true));
	xSetInt(dCollectibles, xCollectibleCount, count);
}

void snarePlayer(int p = 0, int duration = 2000) {
	int timestamp = trTimeMS() + 2000;
	xSetBool(dPlayerData, xPlayerSnared, true);
	if (xGetInt(dPlayerData, xPlayerSnareTime) < timestamp) {
		xSetInt(dPlayerData, xPlayerSnareTime, timestamp);
	}
	trModifyProtounit(xGetString(dPlayerData, xPlayerProto), p, 55, 2);
	xUnitSelectByID(dPlayerData, xPlayerUnitID);
	trMutateSelected(kbGetProtoUnitID(xGetString(dPlayerData, xPlayerProto)));
	if (trCurrentPlayer() == p) {
		trChatSend(0, "<color=1,1,1>You have been snared!</color>");
	}
}

/*
Assume trCurrentPlayer() check has already been performed
Assume dPlayerData pointer is already set
*/
void displayWeapons(int p = 0) {
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase, p);
	xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponTop, p));
	if (trCurrentPlayer() == p) {
		trCounterAbort("dashes");
		string msg = "(Q) Throw | (W) Switch | (E) Dash [";
		int count = xGetInt(dPlayerData, xPlayerDashCount, p);
		if (count < 2) {
			int timediff = (xGetInt(dPlayerData, xPlayerDashCooldown, p) - trTimeMS()) / 1000;
			trCounterAddTime("dashes",timediff,-1,msg + xGetInt(dPlayerData, xPlayerDashCount, p) + "/2]",-1);
		} else {
			trCounterAddTime("dashes",-1,-9999,msg + "2/2]",-1);
		}

		for(i=3; >0) {
			trCounterAbort("weapon"+i);
		}
		if (xGetDatabaseCount(db) > 0) {
			string name = "";
			for(i=xGetDatabaseCount(db); >0) {
				name = weaponName(xGetInt(db, xWeaponType)) + " x" + xGetInt(db, xWeaponCount);
				if (xGetInt(dPlayerData, xPlayerWeaponCurrent, p) == xGetPointer(db)) {
					name = "--> " + name;
				}
				trCounterAddTime("weapon"+i,-1,-9999,name,-1);
				xDatabaseNext(db);
			}
		}
	}
	xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponTop, p));
}

bool pickUpWeapon(int p = 0, int weapon = 0, int count = 0) {
	xSetPointer(dPlayerData, p);
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
	bool done = false;
	if (weapon == WEAPON_WILDCARD) {
		done = true;
		wildcard = p;
		if (trCurrentPlayer() == p) {
			trSoundPlayFN("herocreation.wav","1",-1,"","");
		}
		xUnitSelectByID(dPlayerData, xPlayerUnitID);
		spyEffect(kbGetProtoUnitID("Garrison Flag Sky Passage"), -1, xsVectorSet(dPlayerData, xPlayerSpotlight, p));
	} else {
		xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponCurrent));
		for(i=xGetDatabaseCount(db); >0) {
			xDatabaseNext(db);
			if (xGetInt(db, xWeaponType) == weapon) {
				xSetInt(db, xWeaponCount, count + xGetInt(db, xWeaponCount));
				done = true;
				break;
			}
		}
		if (done == false) {
			if (xGetDatabaseCount(db) < INVENTORY_SIZE) {
				xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponTop));
				xAddDatabaseBlock(db, true);
				xSetInt(db, xWeaponType, weapon);
				xSetInt(db, xWeaponCount, count);
				if (xGetDatabaseCount(db) == 1) {
					xSetInt(dPlayerData, xPlayerWeaponTop, xGetPointer(db));
					xSetInt(dPlayerData, xPlayerWeaponCurrent, xGetPointer(db));
				}
				done = true;
			}
		}
		displayWeapons(p);
	}
	
	return(done);
}

int getCurrentWeaponType() {
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
	int weaponType = -1;
	if (xGetDatabaseCount(db) > 0) {
		weaponType = xGetInt(db, xWeaponType, xGetInt(dPlayerData, xPlayerWeaponCurrent));
	}
	return(weaponType);
}

void switchWeapon(int p = 0) {
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
	if (xGetDatabaseCount(db) > 0) {
		xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponCurrent));
		xSetInt(dPlayerData, xPlayerWeaponCurrent, xDatabaseNext(db));
		displayWeapons(p);
		if (trCurrentPlayer() == p) {
			trSoundPlayFN("ui\jewel.wav","1",-1,"","");
		}
	}
}

void shootWeapon(int p = 0) {
	if (trTimeMS() > xGetInt(dPlayerData, xPlayerShootCooldown)) {
		vector pos = vector(0,0,0);
		xSetInt(dPlayerData, xPlayerShootCooldown, trTimeMS() + SHOOT_COOLDOWN);
		int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
		if (xGetDatabaseCount(db) > 0) {
			xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponCurrent));
			switch(xGetInt(db, xWeaponType))
			{
				case WEAPON_KNIFE:
				{
					shootGenericProj(p, dKnives, "Stymph Bird Feather", xGetVector(dPlayerData, xPlayerThrowPos));
					if (p == wildcard) {
						trUnitHighlight(99.0, false);
					}
					trQuestVarSetFromRand("sound", 1, 5, true);
					xUnitSelectByID(dPlayerData, xPlayerUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("arrow"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
					}
				}
				case WEAPON_GRAPPLING_HOOK:
				{
					shootGrapplingHook(p, xGetVector(dPlayerData, xPlayerThrowPos));
					xUnitSelectByID(dPlayerData, xPlayerUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("petrobolosattack.wav","1",-1,"","");
					}
				}
				case WEAPON_PORTAL:
				{
					shootGenericProj(p, dPortalShots, "Lampades Bolt", xGetVector(dPlayerData, xPlayerThrowPos));
					xUnitSelectByID(dPlayerData, xPlayerUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("lampadesshoot.wav","1",-1,"","");
					}
				}
				case WEAPON_TRAP:
				{
					xAddDatabaseBlock(dTraps, true);
					xSetInt(dTraps, xUnitName, spawnObject(p, "Statue of Automaton Base"));
					xSetInt(dTraps, xUnitID, kbGetBlockID(""+xGetInt(dTraps, xUnitName), true));
					xSetVector(dTraps, xUnitPos, kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName, p), true));
					xSetInt(dTraps, xTrapArmTime, trTimeMS() + 1000);
					xSetInt(dTraps, xUnitOwner, p);
					trUnitHighlight(1.0, false);
					if (trCurrentPlayer() == p) {
						trSoundPlayFN("siegeselect.wav","1",-1,"","");
						trSoundPlayFN("gatherpoint.wav","1",-1,"","");
					}
				}
				case WEAPON_SMOKESCREEN:
				{
					shootGenericProj(p, dSmokeBombs, "Petrobolos Shot", xGetVector(dPlayerData, xPlayerThrowPos));
					xUnitSelectByID(dSmokeBombs, xUnitID);
					trUnitChangeProtoUnit("Petrobolos Shot");
					xUnitSelectByID(dSmokeBombs, xUnitID);
					trUnitOverrideAnimation(0,0,true,false,-1);
					trUnitSetAnimationPath("0,1,0,0,0,0,0");
					xSetVector(dSmokeBombs, xSmokeBombDest, xGetVector(dPlayerData, xPlayerThrowPos));
					xUnitSelectByID(dPlayerData, xPlayerUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("catapultattack.wav","1",-1,"","");
					}
				}
				case WEAPON_SWORD:
				{
					xUnitSelect(dPlayerData, xPlayerDeflector);
					trMutateSelected(kbGetProtoUnitID("Sphinx"));
					trSetSelectedScale(0,0,0);
					trSetSelectedUpVector(0,-1,0);
					trUnitOverrideAnimation(39, 0, false, false, -1);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("sphinxspecialattack.wav","1",-1,"","");
					}
					xSetBool(dPlayerData, xPlayerWhirlwindActive, true);
					xSetInt(dPlayerData, xPlayerWhirlwindTimeout, trTimeMS() + 1000);
					pos = xGetVector(dUnits, xUnitPos, xGetInt(dPlayerData, xPlayerIndex));
					for(x=xGetDatabaseCount(dUnits); >0) {
						xDatabaseNext(dUnits);
						if (p != xGetInt(dUnits, xUnitOwner)) {
							if (targetEligible(p)) {
								if (distanceBetweenVectors(pos, xGetVector(dUnits, xUnitPos)) < 25.0) {
									trQuestVarSetFromRand("sound", 1, 4, true);
									xUnitSelectByID(dUnits, xUnitID);
									if (trUnitVisToPlayer()) {
										trSoundPlayFN("arrowonflesh"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
									}
									trDamageUnit(1);
								}
							}
						}
					}
				}
			}
			xSetInt(db, xWeaponCount, xGetInt(db, xWeaponCount) - 1);
			if (xGetInt(db, xWeaponCount) == 0) {
				xFreeDatabaseBlock(db);
				if (xGetInt(dPlayerData, xPlayerWeaponTop) == xGetInt(dPlayerData, xPlayerWeaponCurrent)) {
					xSetInt(dPlayerData, xPlayerWeaponTop, xGetPointer(db));
				}
				xSetInt(dPlayerData, xPlayerWeaponCurrent, xGetPointer(db));
			}
		}
		displayWeapons(p);
	}
}

void dash(int p = 0) {
	if ((xGetInt(dPlayerData, xPlayerDashCount) > 0) && (xGetBool(dPlayerData, xPlayerSnared) == false)) {
		if (xGetBool(dUnits, xUnitLaunched, xGetInt(dPlayerData, xPlayerIndex, p)) == false) {
			xSetInt(dPlayerData, xPlayerDashCount, xGetInt(dPlayerData, xPlayerDashCount) - 1);
			if (trTimeMS() > xGetInt(dPlayerData, xPlayerDashCooldown)) {
				xSetInt(dPlayerData, xPlayerDashCooldown, trTimeMS() + 15000);
			}
			vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName), true);
			vector prev = pos;
			vector step = getUnitVector(pos, xGetVector(dPlayerData, xPlayerDashPos), 2.0);
			vector dest = pos;
			float dist = xsMin(distanceBetweenVectors(pos, xGetVector(dPlayerData, xPlayerDashPos), false), 8);
			for(i=dist / 2; >0) {
				dest = pos + step;
				if (terrainIsType(vectorToGrid(dest), TERRAIN_WALL, TERRAIN_WALL_SUB)) {
					break;
				} else {
					pos = dest;
					trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(dest),0,xsVectorGetZ(dest),0,true);
					trArmySelect("0,0");
					trUnitChangeProtoUnit("Arkantos Boost SFX");
				}
			}
			step = step * 0.5;
			int next = trGetNextUnitScenarioNameNumber();
			trArmyDispatch(""+p+",0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
			trArmySelect(""+p+",0");
			trMutateSelected(kbGetProtoUnitID("Transport Ship Greek"));
			trSetUnitOrientation(step, vector(0,1,0), true);
			xUnitSelectByID(dPlayerData, xPlayerUnitID);
			trImmediateUnitGarrison(""+next);
			trUnitChangeProtoUnit(xGetString(dPlayerData, xPlayerProto));
			trUnitSelectClear();
			trUnitSelect(""+next, true);
			trUnitChangeProtoUnit("Dust Medium");
			displayWeapons(p);
			if (trCurrentPlayer() == p) {
				trSoundPlayFN("sphinxteleportout.wav","1",-1,"","");
			}
			for(j=xGetDatabaseCount(dUnits); >0) {
				xDatabaseNext(dUnits);
				if (xGetInt(dUnits, xUnitOwner) == p) {
					continue;
				} else if (targetEligible(p)) {
					if (rayCollision(dUnits, prev, step, dist, 4.0)) {
						pos = kbGetBlockPosition(""+xGetInt(dUnits, xUnitName), true);
						trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
						trArmySelect("0,0");
						trUnitChangeProtoUnit("Lightning Sparks");
						xUnitSelectByID(dUnits, xUnitID);
						if (xGetInt(dUnits, xUnitOwner) > 0) {
							trQuestVarSetFromRand("sound", 1, 4, true);
							if (trUnitVisToPlayer()) {
								trSoundPlayFN("arrowonflesh"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
							}
						}
						trDamageUnit(1);
					}
				}
			}
		}
	}
}


rule ysearch
inactive
highFrequency
{
	int id = 0;
	int unittype = 0;
	vector scale = vector(0,0,0);
	vector dest = vector(0,0,0);
	for(i = spysearch; < trGetNextUnitScenarioNameNumber()) {
		id = kbGetBlockID(""+i, true);
		unittype = kbGetUnitBaseTypeID(id);
		switch(unittype)
		{
			case kbGetProtoUnitID("Spy Eye"):
			{
				if (xGetDatabaseCount(dSpyRequests) > 0) {
					scale = xGetVector(dSpyRequests, xSpyRequestScale);
					dest = xGetVector(dSpyRequests, xSpyRequestDest);
					trUnitSelectClear();
					trUnitSelectByID(id);
					trMutateSelected(xGetInt(dSpyRequests, xSpyRequestProto));
					trSetSelectedScale(xsVectorGetX(scale),xsVectorGetY(scale),xsVectorGetZ(scale));
					trUnitOverrideAnimation(xGetInt(dSpyRequests, xSpyRequestAnim),0,true,false,-1);
					trEventFire(xGetInt(dSpyRequests, xSpyRequestEvent));
					if (aiPlanSetUserVariableInt(1*xsVectorGetX(dest),1*xsVectorGetY(dest),1*xsVectorGetZ(dest),i) == false) {
						debugLog("spy error N/A: " + 1*xsVectorGetX(dest) + "," + 1*xsVectorGetY(dest) + "," + 1*xsVectorGetZ(dest));
					}
					xFreeDatabaseBlock(dSpyRequests);
					spyreset = 0;
				} else {
					debugLog("Spy Buffer is empty");
				}
				break;
			}
			case kbGetProtoUnitID("House"):
			{
				xSetVector(dPlayerData, xPlayerThrowPos, kbGetBlockPosition(""+i, true), kbUnitGetOwner(id));
				trUnitSelectClear();
				trUnitSelectByID(id);
				trUnitDestroy();
				break;
			}
			case kbGetProtoUnitID("Storehouse"):
			{
				xSetVector(dPlayerData, xPlayerDashPos, kbGetBlockPosition(""+i, true), kbUnitGetOwner(id));
				trUnitSelectClear();
				trUnitSelectByID(id);
				trUnitDestroy();
				break;
			}
			case kbGetProtoUnitID("Granary"):
			{
				trUnitSelectClear();
				trUnitSelectByID(id);
				trUnitDestroy();
				break;
			}
			case kbGetProtoUnitID("Hero Birth"):
			{
				debugLog("spysearch: " + i);
			}
		}
	}
	if (xGetDatabaseCount(dSpyRequests) > 0) {
		spyreset = spyreset + 1;
		if (spyreset >= 10) {
			debugLog("Spy reset. Failed requests: " + xGetDatabaseCount(dSpyRequests));
			xClearDatabase(dSpyRequests);
			spyreset = 0;
		}
	}
	spysearch = trGetNextUnitScenarioNameNumber();
}