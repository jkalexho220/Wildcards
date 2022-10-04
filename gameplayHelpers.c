int spysearch = 0;
int wildcard = -1;
int wildcardStep = 0;
int wildcardNext = 0;

int knifeCount = 0;
int nextCollectible = 0;

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

void addUnit(int name = 0, int id = 0, int p = 0) {
	xAddDatabaseBlock(dUnits, true);
	xSetInt(dUnits, xUnitName, name);
	xSetInt(dUnits, xUnitID, id);
	xSetInt(dUnits, xUnitOwner, p);
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

	addUnit(xGetInt(dPlayerData, xPlayerUnitName), xGetInt(dPlayerData, xPlayerUnitID), p);
	int x = xsVectorGetX(pos) / 4;
	int y = xsVectorGetZ(pos) / 4;

	addFrontier(x * 2, y * 2);
	cleanFrontier();

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
	} else if (xsVectorGetX(target) > (mapSize * 2)) {
		scale = (xsVectorGetX(target) - (mapSize * 2)) / (xsVectorGetX(target) - xsVectorGetX(from));
		target = xsVectorSet(mapSize * 2,0,xsVectorGetZ(target) + scale * (xsVectorGetZ(from) - xsVectorGetZ(target)));
	}
	
	if (xsVectorGetZ(target) < 0) {
		scale = xsVectorGetZ(target) / (xsVectorGetZ(target) - xsVectorGetZ(from));
		target = xsVectorSet(xsVectorGetX(target) + scale * (xsVectorGetX(from) - xsVectorGetX(target)),0,0);
	} else if (xsVectorGetZ(target) > (mapSize * 2)) {
		scale = (xsVectorGetZ(target) - (mapSize * 2)) / (xsVectorGetZ(target) - xsVectorGetZ(from));
		target = xsVectorSet(xsVectorGetX(target) + scale * (xsVectorGetX(from) - xsVectorGetX(target)),0,mapSize * 2);
	}
	return(target);
}

int spawnObject(int p = 0, string proto = "") {
	int next = trGetNextUnitScenarioNameNumber() + 1;
	xSetPointer(dPlayerData, p);
	xUnitSelect(dPlayerData, xPlayerProjSpawner);
	trUnitChangeProtoUnit("Hero Norse");

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

void shootGenericProj(int p = 0, int db = 0, string proto = "", vector dest = vector(0,0,0)) {
	vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName, p), true);
	vector dir = getUnitVector(pos, dest);

	xAddDatabaseBlock(db, true);
	xSetInt(db, xUnitName, spawnObject(p, proto));
	xSetInt(db, xUnitID, kbGetBlockID(""+xGetInt(db, xUnitName), true));
	xSetInt(db, xUnitOwner, p);
	xSetVector(db, xProjPrev, pos);
	xSetVector(db, xProjDir, dir);
	
	dest = vectorSetAsTargetVector(pos, dir, 999.0);
	xUnitSelectByID(db, xUnitID);
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
	xSetVector(dCollectibles, xUnitPos, pos);
	xSetInt(dCollectibles, xCollectibleCount, count);
}


/*
Assume trCurrentPlayer() check has already been performed
Assume dPlayerData pointer is already set
*/
void displayWeapons() {
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
	for(i=3; >0) {
		trCounterAbort("weapon"+i);
	}
	trClearCounterDisplay();
	if (xGetDatabaseCount(db) > 0) {
		bool first = true;
		string name = "";
		for(i=xGetDatabaseCount(db); >0) {
			name = weaponName(xGetInt(db, xWeaponType)) + " x" + xGetInt(db, xWeaponCount);
			if (first) {
				first = false;
				name = "(Q) " + name;
			}
			trCounterAddTime("weapon"+i,-1,-9999,name,-1);
			xDatabaseNext(db);
		}
		if (xGetDatabaseCount(db) > 1) {
			trSetCounterDisplay("(W) Switch Item");
		}
	}
}

bool pickUpWeapon(int p = 0, int weapon = 0, int count = 0) {
	if (weapon == WEAPON_KNIFE) {
		knifeCount = knifeCount + 1;
	}
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
				xAddDatabaseBlock(db, true);
				xSetInt(db, xWeaponType, weapon);
				xSetInt(db, xWeaponCount, count);
				if (xGetDatabaseCount(db) == 1) {
					xSetInt(dPlayerData, xPlayerWeaponCurrent, xGetPointer(db));
				}
				done = true;
			}
		}
		xSetPointer(db, xGetInt(dPlayerData, xPlayerWeaponCurrent));
		if (trCurrentPlayer() == p) {
			displayWeapons();
		}
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
		if (trCurrentPlayer() == p) {
			displayWeapons();
			trSoundPlayFN("ui\jewel.wav","1",-1,"","");
		}
	}
}

void shootWeapon(int p = 0) {
	if (trTimeMS() > xGetInt(dPlayerData, xPlayerShootCooldown)) {
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
			}
			xSetInt(db, xWeaponCount, xGetInt(db, xWeaponCount) - 1);
			if (xGetInt(db, xWeaponCount) == 0) {
				xFreeDatabaseBlock(db);
				xSetInt(dPlayerData, xPlayerWeaponCurrent, xGetPointer(db));
			}
		}
		if (trCurrentPlayer() == p) {
			displayWeapons();
		}
	}
}

void displayDashCount() {
	trCounterAbort("dashes");
	string msg = "(E) Dash [";
	int count = xGetInt(dPlayerData, xPlayerDashCount);
	if (count < 2) {
		int timediff = (xGetInt(dPlayerData, xPlayerDashCooldown) - trTimeMS()) / 1000;
		trCounterAddTime("dashes",timediff,0,msg + xGetInt(dPlayerData, xPlayerDashCount) + "/2]",-1);
	} else {
		trCounterAddTime("dashes",-1,-9999,msg + "2/2]",-1);
	}
}

void dash(int p = 0) {
	if (xGetInt(dPlayerData, xPlayerDashCount) > 0) {
		xSetInt(dPlayerData, xPlayerDashCount, xGetInt(dPlayerData, xPlayerDashCount) - 1);
		if (trTimeMS() > xGetInt(dPlayerData, xPlayerDashCooldown)) {
			xSetInt(dPlayerData, xPlayerDashCooldown, trTimeMS() + 15000);
		}
		vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName), true);
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
		int next = trGetNextUnitScenarioNameNumber();
		trArmyDispatch(""+p+",0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
		trArmySelect(""+p+",0");
		trMutateSelected(kbGetProtoUnitID("Transport Ship Greek"));
		trSetUnitOrientation(step * 0.5, vector(0,1,0), true);
		xUnitSelectByID(dPlayerData, xPlayerUnitID);
		trImmediateUnitGarrison(""+next);
		trUnitChangeProtoUnit(xGetString(dPlayerData, xPlayerProto));
		trUnitSelectClear();
		trUnitSelect(""+next, true);
		trUnitChangeProtoUnit("Dust Medium");
		if (trCurrentPlayer() == p) {
			trSoundPlayFN("sphinxteleportout.wav","1",-1,"","");
			displayDashCount();
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
	spysearch = trGetNextUnitScenarioNameNumber();
}