int spysearch = 0;
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

void spawnPlayer(int p = 0, vector pos = vector(0,0,0)) {
	int old = xGetPointer(dPlayerData);
	xSetPointer(dPlayerData, p);
	xSetInt(dPlayerData, xPlayerUnitName, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+p+",0", xGetString(dPlayerData, xPlayerProto), 1, xsVectorGetX(pos), 0, xsVectorGetZ(pos), 0, true);
	xSetInt(dPlayerData, xPlayerUnitID, kbGetBlockID(""+xGetInt(dPlayerData, xPlayerUnitName), true));
	xSetBool(dPlayerData, xPlayerAlive, true);
	xSetVector(dPlayerData, xPlayerPos, pos);
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



/*
Assume trCurrentPlayer() check has already been performed
Assume dPlayerData pointer is already set
*/
void displayWeapons() {
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
	for(i=3; >0) {
		trCounterAbort("weapon"+i);
	}
	if (xGetDatabaseCount(db) > 0) {
		bool first = true;
		string name = "";
		for(i=xGetDatabaseCount(db); >0) {
			name = weaponName(xGetInt(db, xWeaponType)) + " x" + xGetInt(db, xWeaponCount);
			if (first) {
				first = false;
				name = "> " + name + " <";
			}
			trCounterAddTime("weapon"+i,-1,-9999,name,-1);
			xDatabaseNext(db);
		}
	}
}

bool pickUpWeapon(int p = 0, int weapon = 0, int count = 0) {
	xSetPointer(dPlayerData, p);
	int db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
	bool done = false;
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
	if (trCurrentPlayer() == p) {
		displayWeapons();
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

void dash(int p = 0) {
	if (trCurrentPlayer() == p) {
		displayWeapons();
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