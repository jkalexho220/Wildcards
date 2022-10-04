void removeCamTracks(int eventID = -1) {
	trackRemove();
	trUnblockAllSounds();
}

void hotkeyAbility(int ability = 0) {
	xsSetContextPlayer(0);
	int old = xGetPointer(dPlayerData);
	xSetPointer(dPlayerData, trCurrentPlayer());
	if (xGetBool(dPlayerData, xPlayerAlive)) {
		trackInsert();
		trackAddWaypoint();
		trackPlay(1,EVENT_BUILD_AT_CURSOR);
		trBlockAllSounds(false);
		xUnitSelectByID(dPlayerData, xPlayerUnitID);
		if (trUnitIsSelected() == false) {
			uiFindType(xGetString(dPlayerData, xPlayerProto));
		}
		switch(ability)
		{
			case EVENT_BUILD_HOUSE:
			{
				uiSetProtoCursor("House", true);
			}
			case EVENT_BUILD_GRANARY:
			{
				uiSetProtoCursor("Granary", true);
			}
			case EVENT_BUILD_STOREHOUSE:
			{
				uiSetProtoCursor("Storehouse", true);
			}
		}
	}
	xSetPointer(dPlayerData, old);
}

void buildAtCursor(int eventID = -1) {
	uiBuildAtPointer();
	uiClearCursor();
	trackInsert();
	trackAddWaypoint();
	trackPlay(1,EVENT_REMOVE_CAM_TRACKS);
}

rule gameplay_setup
inactive
highFrequency
{
	xsDisableSelf();
	startPos = perlinRoll(perlin, mapSize / 2, mapSize / 2, 1, -10.0);

	xsEnableRule("gameplay_start");

	// spawn the hotkey units
	for(i=EVENT_BUILD_HOUSE; <= EVENT_BUILD_STOREHOUSE) {
		trEventSetHandler(i, "hotkeyAbility");
	}
	trEventSetHandler(EVENT_REMOVE_CAM_TRACKS, "removeCamTracks");
	trEventSetHandler(EVENT_BUILD_AT_CURSOR, "buildAtCursor");

	map("q", "game", "trackInsert(); trackAddWaypoint();trackPlay(-1,0);");
	map("w", "game", "trackInsert(); trackAddWaypoint();trackPlay(-1,1);");
	map("e", "game", "trackInsert(); trackAddWaypoint();trackPlay(-1,2);");

	vector dir = vector(1,0,0);
	float angle = 6.283185 / (cNumberPlayers - 1);
	float mCos = xsCos(angle);
	float mSin = xsSin(angle);
	for(p=1; < cNumberPlayers) {
		spawnPlayer(p, startPos);
		trUnitSelectClear();
		trUnitSelectByID(xGetInt(dPlayerData, xPlayerUnitID, p));
		trUnitTeleport(xsVectorGetX(startPos) + (xsVectorGetX(dir) * 3.0), 0, xsVectorGetZ(startPos) + (xsVectorGetZ(dir) * 3.0));
		trSetUnitOrientation(dir, vector(0,1,0), true);
		dir = rotationMatrix(dir, mCos, mSin);
		pickUpWeapon(p, WEAPON_KNIFE, 5);
	}
}

rule gameplay_start
inactive
highFrequency
{
	xsDisableSelf();
	spysearch = trGetNextUnitScenarioNameNumber();

	xsEnableRule("track_los");
	xsEnableRule("ysearch");
	xsEnableRule("gameplay_always");

	trSetFogAndBlackmap(true, false);
	trCameraCut(vector(-46.464447,70.710701,-46.464447) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));

	trSetCounterDisplay("(W) Switch Item");
}

rule gameplay_always
inactive
highFrequency
{
	float dist = 0;
	int pointer = 0;
	int p = 0;
	vector pos = vector(0,0,0);
	vector prev = vector(0,0,0);
	vector dir = vector(0,0,0);

	for(p=1; < cNumberPlayers) {
		xSetPointer(dPlayerData, p);
		if (trPlayerResourceCount(p, "food") > 0) {
			trPlayerGrantResources(p, "food", -9999);
			shootWeapon(p);
		}
		if (trPlayerResourceCount(p, "wood") > 0) {
			trPlayerGrantResources(p, "wood", -9999);
			switchWeapon(p);
		}
		if (trPlayerResourceCount(p, "gold") > 0) {
			trPlayerGrantResources(p, "gold", -9999);
			dash(p);
		}
	}

	// collision position update
	for(i=xGetDatabaseCount(dUnits); >0) {
		xDatabaseNext(dUnits);
		xUnitSelectByID(dUnits, xUnitID);
		if (trUnitAlive() == false) {
			xFreeDatabaseBlock(dUnits);
		} else {
			xSetVector(dUnits, xUnitPos, kbGetBlockPosition(""+xGetInt(dUnits, xUnitName), true));
		}
	}

	for(i=xGetDatabaseCount(dKnives); >0) {
		xDatabaseNext(dKnives);
		pos = kbGetBlockPosition(""+xGetInt(dKnives, xUnitName), true);
		xSetVector(dKnives, xUnitPos, pos);
		if (terrainIsType(vectorToGrid(pos), TERRAIN_WALL, TERRAIN_WALL_SUB)) {
			spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir) * 2.0), WEAPON_KNIFE);
			trQuestVarSetFromRand("sound", 1, 3, true);
			xUnitSelectByID(dKnives, xUnitID);
			if (trUnitVisToPlayer()) {
				trSoundPlayFN("mine"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
			}
			trUnitChangeProtoUnit("Lightning Sparks ground");
			xFreeDatabaseBlock(dKnives);
		}
	}

	// knife collision detection
	bool hit = false;
	for(i=xGetDatabaseCount(dKnives); >0) {
		xDatabaseNext(dKnives);
		pos = xGetVector(dKnives, xUnitPos);
		prev = xGetVector(dKnives, xProjPrev);
		dist = distanceBetweenVectors(pos, prev);
		if (dist > 1.0) {
			p = xGetInt(dKnives, xUnitOwner);
			dist = xsSqrt(dist);
			dir = xGetVector(dKnives, xProjDir);
			for(j=xGetDatabaseCount(dUnits); >0) {
				xDatabaseNext(dUnits);
				if (xGetInt(dUnits, xUnitOwner) == p) {
					continue;
				} else if (rayCollision(dUnits, prev, dir, dist, xGetFloat(dKnives, xProjRadius))) {
					xUnitSelectByID(dUnits, xUnitID);
					trDamageUnit(1);
					xUnitSelectByID(dKnives, xUnitID);
					if (xGetInt(dUnits, xUnitOwner) > 0) {
						trQuestVarSetFromRand("sound", 1, 4, true);
						if (trUnitVisToPlayer()) {
							trSoundPlayFN("arrowonflesh"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
						}
					}
					trUnitChangeProtoUnit("Lightning Sparks ground");
					xFreeDatabaseBlock(dKnives);
					hit = true;
					break;
				}
			}
			if (hit == false) {
				pointer = xGetPointer(dKnives);
				for(j=xGetDatabaseCount(dKnives); >1) {
					xDatabaseNext(dKnives);
					if (rayCollision(dKnives, prev, dir, dist, 1.0)) {
						xUnitSelectByID(dKnives, xUnitID);
						trUnitChangeProtoUnit("Lightning Sparks ground");
						spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir) * 2.0), WEAPON_KNIFE);
						xFreeDatabaseBlock(dKnives);
						hit = true;
						break;
					}
				}
				xSetPointer(dKnives, pointer);
				if (hit) {
					trQuestVarSetFromRand("sound", 1, 6, true);
					xUnitSelectByID(dKnives, xUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("metalslice"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
					}
					trUnitChangeProtoUnit("Lightning Sparks ground");
					spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir) * 2.0), WEAPON_KNIFE);
					xFreeDatabaseBlock(dKnives);
				}
			}
			if (hit == false) {
				xSetVector(dKnives, xProjPrev, pos);
			}
		}
	}
}