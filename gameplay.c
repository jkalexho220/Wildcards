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

	xsEnableRule("ysearch");

	//xsEnableRule("gameplay_start");

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
	trQuestVarSetFromRand("sound", 0, 6.283185, false);
	angle = trQuestVarGet("sound");
	mCos = xsCos(angle);
	mSin = xsSin(angle);
	vector pos = perlinRoll(perlin, mapSize / 2 * (1.0 + mCos * 0.8), mapSize / 2 * (1.0 + mSin * 0.8), 1, -5.0);
	spawnCollectible(pos, WEAPON_WILDCARD, 1);
}

rule gameplay_start
inactive
highFrequency
{
	xsDisableSelf();
	spysearch = trGetNextUnitScenarioNameNumber();

	xsEnableRule("track_los");
	xsEnableRule("gameplay_always");

	trSetFogAndBlackmap(true, false);
	trCameraCut(vector(-46.464447,70.710701,-46.464447) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));

	for(p=1; < cNumberPlayers) {
		displayWeapons(p);
	}

	trMusicPlayCurrent();
	trPlayNextMusicTrack();

	trMessageSetText("Find the Spotlight to become the Wildcard!");
}

int lastTimestamp = 0;

rule gameplay_always
inactive
highFrequency
{
	float dist = 0;
	int pointer = 0;
	int p = 0;
	int val = 0;
	int db = 0;
	int x = 0;
	int y = 0;
	bool hit = false;
	vector start = vector(0,0,0);
	vector pos = vector(0,0,0);
	vector prev = vector(0,0,0);
	vector dir = vector(0,0,0);
	string proto = "";

	float timediff = 0.001 * (trTimeMS() - lastTimestamp);
	lastTimestamp = trTimeMS();


	// collision position update
	for(i=xGetDatabaseCount(dUnits); >0) {
		xDatabaseNext(dUnits);
		xUnitSelectByID(dUnits, xUnitID);
		if (trUnitAlive() == false) {
			xFreeDatabaseBlock(dUnits);
		} else if (xGetBool(dUnits, xUnitStationary) == false) {
			xSetVector(dUnits, xUnitPos, kbGetBlockPosition(""+xGetInt(dUnits, xUnitName), true));
		}
	}

	for(p=1; < cNumberPlayers) {
		xSetPointer(dPlayerData, p);
		if (xGetBool(dPlayerData, xPlayerAlive)) {
			xUnitSelectByID(dPlayerData, xPlayerUnitID);
			if (trUnitAlive() == false) {
				// death code goes here
				removeFrontier(0, 0, true);
				xSetBool(dPlayerData, xPlayerAlive, false);
				db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
				xClearDatabase(db);
				xSetInt(dPlayerData, xPlayerDashStep, 0);
				xSetInt(dPlayerData, xPlayerRespawnTime, trTimeMS() + 15000);
				displayWeapons(p);
				if (trCurrentPlayer() == p) {
					trCounterAbort("dashes");
					trCounterAddTime("respawn", 15, 1, "Respawn", -1);
				}
				if (wildcard == p) {
					trMessageSetText("The Wildcard has been dropped! Treasure locations have been reset!");
					trSoundPlayFN("backtowork.wav","1",-1,"","");
					trOverlayText("",1,-1,-1,-1);
					for(i=xGetDatabaseCount(dGoldMines); >0) {
						xDatabaseNext(dGoldMines);
						removeGoldMine();
					}
					wildcard = 0;
					wildcardStep = 0;
					spawnCollectible(xGetVector(dPlayerData, xPlayerPos), WEAPON_WILDCARD, 1);
				}
			} else {
				switch(xGetInt(dPlayerData, xPlayerDashStep))
				{
					case 0:
					{
						if (xGetInt(dPlayerData, xPlayerDashCount) < 2) {
							xSetInt(dPlayerData, xPlayerDashCooldown, trTimeMS() + 15000);
							displayWeapons(p);
							xSetInt(dPlayerData, xPlayerDashStep, 1);
						}
					}
					case 1:
					{
						if (trTimeMS() > xGetInt(dPlayerData, xPlayerDashCooldown)) {
							xSetInt(dPlayerData, xPlayerDashStep, 0);
							xSetInt(dPlayerData, xPlayerDashCount, 1 + xGetInt(dPlayerData, xPlayerDashCount));
							displayWeapons(p);
						}
					}
				}
				// abilities
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
				// snare
				if (xGetBool(dPlayerData, xPlayerSnared)) {
					if (trTimeMS() > xGetInt(dPlayerData, xPlayerSnareTime)) {
						xSetBool(dPlayerData, xPlayerSnared, false);
						trModifyProtounit(xGetString(dPlayerData, xPlayerProto), p, 55, 1);
					}
				}

				pos = xGetVector(dUnits, xUnitPos, xGetInt(dPlayerData, xPlayerIndex));
				x = xsVectorGetX(pos) / 2;
				y = xsVectorGetZ(pos) / 2;
				dist = 5.0 * xsPow(0.5, aiPlanGetUserVariableInt(frostArray, x, y));
				trModifyProtounit(xGetString(dPlayerData, xPlayerProto), p, 1, dist - xGetFloat(dPlayerData, xPlayerSpeed));
				xSetFloat(dPlayerData, xPlayerSpeed, dist);

				if (xGetBool(dPlayerData, xPlayerWhirlwindActive)) {
					if (trTimeMS() > xGetInt(dPlayerData, xPlayerWhirlwindTimeout)) {
						xSetBool(dPlayerData, xPlayerWhirlwindActive, false);
						xUnitSelect(dPlayerData, xPlayerDeflector);
						trMutateSelected(kbGetProtoUnitID("Cinematic Block"));
					} else {
						for(i=xGetDatabaseCount(dKnives); >0) {
							xDatabaseNext(dKnives);
							if (xGetInt(dKnives, xUnitOwner) != p) {
								if (distanceBetweenVectors(xGetVector(dKnives, xUnitPos), pos) < 25.0) {
									spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xUnitPos)), WEAPON_KNIFE);
									trQuestVarSetFromRand("sound", 1, 6, true);
									xUnitSelectByID(dKnives, xUnitID);
									if (trUnitVisToPlayer()) {
										trSoundPlayFN("metalslice"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
									}
									trUnitChangeProtoUnit("Lightning Sparks ground");
									xFreeDatabaseBlock(dKnives);
								}
							}
						}
					}
				}
			}
		} else if (trTimeMS() > xGetInt(dPlayerData, xPlayerRespawnTime)) {
			prev = xsVectorSet(mapSize, 0, mapSize);
			if (wildcard > 0) {
				pos = xGetVector(dPlayerData, xPlayerPos, wildcard);
			} else {
				pos = xGetVector(dPlayerData, xPlayerPos);
			}
			dir = getUnitVector(prev, pos);
			trQuestVarSetFromRand("sound", 1.5708, 4.712388, false);
			dir = rotationMatrix(dir, xsCos(trQuestVarGet("sound")), xsSin(trQuestVarGet("sound")));
			pos = vectorToGrid(prev + dir * 0.8 * mapSize);
			spawnPlayer(p, perlinRoll(perlin, xsVectorGetX(pos), xsVectorGetZ(pos), 2, -2.0));
			pickUpWeapon(p, WEAPON_KNIFE, 5);
			pickUpWeapon(p, WEAPON_PORTAL, 1);
			if (trCurrentPlayer() == p) {
				proto = xGetString(dPlayerData, xPlayerProto, p);
				uiFindType(proto);
				trSoundPlayFN("herorevived.wav","1",-1,"","");
			}
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
	hit = false;
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
				} else if (targetEligible(p)) {
					if (rayCollision(dUnits, prev, dir, dist, xGetFloat(dKnives, xProjRadius))) {
						xUnitSelectByID(dUnits, xUnitID);
						trDamageUnit(1);
						xUnitSelectByID(dKnives, xUnitID);
						if (xGetInt(dUnits, xUnitOwner) > 0) {
							trQuestVarSetFromRand("sound", 1, 4, true);
							if (trUnitVisToPlayer()) {
								trSoundPlayFN("arrowonflesh"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
							}
						} else {
							spawnCollectible(vectorSnapToGrid(prev - dir), WEAPON_KNIFE);
						}
						xUnitSelectByID(dKnives, xUnitID);
						trUnitChangeProtoUnit("Lightning Sparks ground");
						xFreeDatabaseBlock(dKnives);
						hit = true;
						break;
					}
				}
			}
			if (hit == false) {
				pointer = xGetPointer(dKnives);
				for(j=xGetDatabaseCount(dKnives); >1) {
					xDatabaseNext(dKnives);
					if (xGetInt(dKnives, xUnitOwner) != p) {
						if (rayCollision(dKnives, prev, dir, dist, xGetFloat(dKnives, xProjRadius))) {
							xUnitSelectByID(dKnives, xUnitID);
							trUnitChangeProtoUnit("Lightning Sparks ground");
							spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir)), WEAPON_KNIFE);
							xFreeDatabaseBlock(dKnives);
							debugLog("first knife gone");
							hit = true;
							break;
						}
					}
				}
				xSetPointer(dKnives, pointer);
				if (hit) {
					debugLog("second knife gone");
					trQuestVarSetFromRand("sound", 1, 6, true);
					xUnitSelectByID(dKnives, xUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("metalslice"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
					}
					trUnitChangeProtoUnit("Lightning Sparks ground");
					spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir)), WEAPON_KNIFE);
					xFreeDatabaseBlock(dKnives);
					break;
				}
			}
			if (hit == false) {
				xSetVector(dKnives, xProjPrev, pos);
			}
		}
	}

	// grappling collision detection
	for(i=xGetDatabaseCount(dGrapplingHooks); >0) {
		xDatabaseNext(dGrapplingHooks);
		xSetVector(dGrapplingHooks, xUnitPos, kbGetBlockPosition(""+xGetInt(dGrapplingHooks, xUnitName), true));
		switch(xGetInt(dGrapplingHooks, xGrapplingHookStep))
		{
			case 0:
			{
				if (xGetDatabaseCount(dSpyRequests) == 0) {
					xSetInt(dGrapplingHooks, xGrapplingHookStep, 1);
					xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
					trMutateSelected(kbGetProtoUnitID("Torch"));
					trUnitSetAnimationPath("2,0,1,1,1,0,0");
					trSetSelectedUpVector(0,-1,0);
				}
			}
			case 1:
			{
				p = xGetInt(dGrapplingHooks, xUnitOwner);
				pos = xGetVector(dGrapplingHooks, xUnitPos);
				start = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName, p), true);
				dist = distanceBetweenVectors(pos, start, false) / 6.0;
				dir = getUnitVector(pos, start, dist);
				// draw the lines
				xUnitSelect(dGrapplingHooks, xGrapplingHookLineProj);
				trSetSelectedUpVector(xsVectorGetX(dir), -0.99, xsVectorGetZ(dir));
				xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
				trSetSelectedUpVector(0.0 - xsVectorGetX(dir), -0.99, 0.0 - xsVectorGetZ(dir));

				prev = xGetVector(dGrapplingHooks, xProjPrev);
				dist = distanceBetweenVectors(pos, prev);
				if (terrainIsType(vectorToGrid(pos), TERRAIN_WALL, TERRAIN_WALL_SUB)) {
					pointer = xGetInt(dPlayerData, xPlayerIndex, p);
					xSetPointer(dUnits, pointer);
					if (xGetBool(dPlayerData, xPlayerAlive, p) && (xGetBool(dUnits, xUnitLaunched) == false)) {
						xSetInt(dGrapplingHooks, xGrapplingHookStep, 2);
						xUnitSelectByID(dGrapplingHooks, xUnitID);
						trMutateSelected(kbGetProtoUnitID("Wadjet Spit"));
						xSetInt(dGrapplingHooks, xGrapplingHookIndex, pointer);
						xSetInt(dGrapplingHooks, xGrapplingHookTarget, xGetInt(dPlayerData, xPlayerUnitID, p));
						launchUnit(dUnits, prev);
						if (trUnitVisToPlayer()) {
							trSoundPlayFN("wall.wav","1",-1,"","");
						}
					} else {
						xUnitSelect(dGrapplingHooks, xGrapplingHookLineProj);
						trUnitDestroy();
						xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
						trUnitDestroy();
						xUnitSelectByID(dGrapplingHooks, xUnitID);
						trUnitChangeProtoUnit("Dust Small");
						xFreeDatabaseBlock(dGrapplingHooks);
					}
				} else if (dist > 1.0) {
					p = xGetInt(dGrapplingHooks, xUnitOwner);
					dist = xsSqrt(dist);
					dir = xGetVector(dGrapplingHooks, xProjDir);
					for(j=xGetDatabaseCount(dUnits); >0) {
						xDatabaseNext(dUnits);
						if (xGetInt(dUnits, xUnitOwner) == p) {
							continue;
						} else if (xGetInt(dUnits, xUnitOwner) > 0) {
							if (rayCollision(dUnits, prev, dir, dist, xGetFloat(dGrapplingHooks, xProjRadius))) {
								trQuestVarSetFromRand("sound", 1, 2, true);
								if (trUnitVisToPlayer()) {
									trSoundPlayFN("titanpunch"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
								}
								xSetInt(dGrapplingHooks, xGrapplingHookStep, 3);
								launchUnit(dUnits, start);
								
								xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
								trUnitDestroy();
								xUnitSelect(dGrapplingHooks, xGrapplingHookLineProj);
								trUnitDestroy();
								xUnitSelectByID(dGrapplingHooks, xUnitID);
								trUnitChangeProtoUnit("Dust Small");
								
								xSetInt(dGrapplingHooks, xGrapplingHookIndex, xGetPointer(dUnits));
								xSetInt(dGrapplingHooks, xGrapplingHookTarget, xGetInt(dUnits, xUnitID));
								xSetVector(dGrapplingHooks, xProjPrev, start);
								
								xUnitSelectByID(dUnits, xUnitID);
								spyEffect(kbGetProtoUnitID("Cinematic Block"), -1, xsVectorSet(dGrapplingHooks, xGrapplingHookLinePlayer, xGetPointer(dGrapplingHooks)));
								
								hit = true;
								break;
							}
						}
					}
					if (hit == false) {
						xSetVector(dGrapplingHooks, xProjPrev, pos);
					}
				}
			}
			case 2: // pull self to wall
			{
				xUnitSelectByID(dGrapplingHooks, xGrapplingHookTarget);
				if (trUnitAlive() && xGetBool(dUnits, xUnitLaunched, xGetInt(dGrapplingHooks, xGrapplingHookIndex))) {
					p = xGetInt(dGrapplingHooks, xUnitOwner);
					pos = xGetVector(dGrapplingHooks, xUnitPos);
					start = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName, p), true);
					dist = distanceBetweenVectors(pos, start, false) / 6.0;
					dir = getUnitVector(pos, start, dist);
					// draw the lines
					xUnitSelect(dGrapplingHooks, xGrapplingHookLineProj);
					trSetSelectedUpVector(xsVectorGetX(dir), -0.99, xsVectorGetZ(dir));
					xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
					trSetSelectedUpVector(0.0 - xsVectorGetX(dir), -0.99, 0.0 - xsVectorGetZ(dir));
				} else {
					debugLog("done");
					xUnitSelect(dGrapplingHooks, xGrapplingHookLineProj);
					trUnitDestroy();
					xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
					trUnitDestroy();
					xUnitSelectByID(dGrapplingHooks, xUnitID);
					trUnitChangeProtoUnit("Dust Small");
					xFreeDatabaseBlock(dGrapplingHooks);
				}
			}
			case 3: // pull enemy to self
			{
				if (xGetDatabaseCount(dSpyRequests) == 0) {
					xSetInt(dGrapplingHooks, xGrapplingHookStep, 4);
					xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
					trMutateSelected(kbGetProtoUnitID("Torch"));
					trUnitSetAnimationPath("2,0,1,1,1,0,0");
					trSetSelectedUpVector(0,-1,0);
				}
			}
			case 4:
			{
				xUnitSelectByID(dGrapplingHooks, xGrapplingHookTarget);
				if (trUnitAlive() && xGetBool(dUnits, xUnitLaunched, xGetInt(dGrapplingHooks, xGrapplingHookIndex))) {
					pointer = xGetInt(dGrapplingHooks, xGrapplingHookIndex);
					pos = xGetVector(dUnits, xUnitPos, pointer);
					start = xGetVector(dGrapplingHooks, xProjPrev);
					dist = distanceBetweenVectors(pos, start, false) / 6.0;
					dir = getUnitVector(pos, start, dist);
					// draw the lines
					xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
					trSetSelectedUpVector(xsVectorGetX(dir), -0.99, xsVectorGetZ(dir));
				} else {
					debugLog("done");
					xUnitSelect(dGrapplingHooks, xGrapplingHookLinePlayer);
					trUnitDestroy();
					xFreeDatabaseBlock(dGrapplingHooks);
				}
			}
		}
	}

	// trap collision detection
	for (i=xsMin(xGetDatabaseCount(dTraps), 5); > 0) {
		xDatabaseNext(dTraps);
		if (xGetBool(dTraps, xTrapArmed)) {
			prev = xGetVector(dTraps, xUnitPos);
			for(p=1; < cNumberPlayers) {
				if (p != xGetInt(dTraps, xUnitOwner)) {
					xSetPointer(dPlayerData, p);
					if (xGetBool(dPlayerData, xPlayerAlive)) {
						pos = xGetVector(dUnits, xUnitPos, xGetInt(dPlayerData, xPlayerIndex));
						if (distanceBetweenVectors(pos, prev) < 3.0) {
							snarePlayer(p, 2000);
							xUnitSelectByID(dTraps, xUnitID);
							trQuestVarSetFromRand("sound", 1, 3, true);
							if (trUnitVisToPlayer()) {
								trSoundPlayFN("crushmetal"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
							}
							trUnitChangeProtoUnit("Dust Large");
							xFreeDatabaseBlock(dTraps);
						}
					}
				}
			}
		} else if (trTimeMS() > xGetInt(dTraps, xTrapArmTime)) {
			xSetBool(dTraps, xTrapArmed, true);
			xUnitSelectByID(dTraps, xUnitID);
			if (trCurrentPlayer() == xGetInt(dTraps, xUnitOwner)) {
				trUnitHighlight(9999, false);
			}
		}
	}

	for(i=xGetDatabaseCount(dPortalShots); >0) {
		xDatabaseNext(dPortalShots);
		xSetVector(dPortalShots, xUnitPos, kbGetBlockPosition(""+xGetInt(dPortalShots, xUnitName), true));
		dir = xGetVector(dPortalShots, xProjDir) * 2.0;
		pos = xGetVector(dPortalShots, xUnitPos) + dir;
		if (terrainIsType(vectorToGrid(pos), TERRAIN_WALL, TERRAIN_WALL_SUB)) {
			hit = false;
			for(j=mapSize * mapSize; >0) {
				pos = pos + dir;
				if (xsVectorGetX(pos) < 0 || xsVectorGetZ(pos) < 0 || xsVectorGetX(pos) > (2.0 * mapSize) || xsVectorGetZ(pos) > (2.0 * mapSize)) {
					break;
				} else if (terrainIsType(vectorToGrid(pos), TERRAIN_WALL, TERRAIN_WALL_SUB) == false) {
					hit = true;
					break;
				}
			}
			if (hit) {
				xUnitSelectByID(dPortalShots, xUnitID);
				if (trUnitVisToPlayer()) {
					trSoundPlayFN("skypassageout.wav","1",-1,"","");
				}
				trUnitDestroy();
				prev = xGetVector(dPortalShots, xUnitPos);
				dir = getUnitVector(prev, pos);

				pos = vectorSnapToGrid(pos + dir * 2.0);
				prev = vectorSnapToGrid(prev - dir * 2.0);
				dir = getUnitVector(prev, pos);
				
				pointer = trGetNextUnitScenarioNameNumber();
				xAddDatabaseBlock(dPortals, true);
				xSetInt(dPortals, xPortalUnitsStart, pointer);

				trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(prev),0,xsVectorGetZ(prev),0,true);
				trUnitSelectClear();
				trUnitSelect(""+pointer, true);
				trMutateSelected(kbGetProtoUnitID("Vortex Finish Linked"));
				trUnitSetAnimationPath("0,0,1,0,0,0,0");
				trSetUnitOrientation(vector(0,0,0) - dir, vector(0,1,0), true);

				pointer = trGetNextUnitScenarioNameNumber();
				trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(prev),0,xsVectorGetZ(prev),0,true);
				trUnitSelectClear();
				trUnitSelect(""+pointer, true);
				trMutateSelected(kbGetProtoUnitID("Vortex Start Linked"));
				trSetSelectedUpVector(xsVectorGetX(dir), -1.0, xsVectorGetZ(dir));
				trUnitSetAnimationPath("0,0,1,0,0,0,0");

				pointer = trGetNextUnitScenarioNameNumber();
				trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
				trUnitSelectClear();
				trUnitSelect(""+pointer, true);
				trMutateSelected(kbGetProtoUnitID("Vortex Start Linked"));
				trSetSelectedUpVector(xsVectorGetX(dir), -1.0, xsVectorGetZ(dir));
				trUnitSetAnimationPath("0,0,1,0,0,0,0");

				xSetInt(dPortals, xPortalUnitsEnd, trGetNextUnitScenarioNameNumber());
				pointer = trGetNextUnitScenarioNameNumber();
				trArmyDispatch("0,0","Dwarf",1,xsVectorGetX(pos),0,xsVectorGetZ(pos),0,true);
				trUnitSelectClear();
				trUnitSelect(""+pointer, true);
				trMutateSelected(kbGetProtoUnitID("Vortex Finish Linked"));
				trUnitSetAnimationPath("0,0,1,0,0,0,0");
				trSetUnitOrientation(dir, vector(0,1,0), true);
				
				xSetVector(dPortals, xPortalStart, xGetVector(dPortalShots, xUnitPos));
				xSetVector(dPortals, xPortalEnd, pos + dir * 2.0);
				xSetInt(dPortals, xPortalTimeout, trTimeMS() + 30000);
				
			} else {
				xUnitSelectByID(dPortalShots, xUnitID);
				if (trUnitVisToPlayer()) {
					trSoundPlayFN("cantdothat.wav","1",-1,"","");
				}
				trUnitDestroy();
			}
			xFreeDatabaseBlock(dPortalShots);
		}
	}
	// Collectibles
	for(i=xsMin(5, xGetDatabaseCount(dCollectibles)); >0) {
		xDatabaseNext(dCollectibles);
		xUnitSelect(dCollectibles, xCollectibleObject);
		if (trUnitIsSelected()) {
			displayWeaponDetails(xGetInt(dCollectibles, xCollectibleType));
			reselectMyself();
		}
		for(p=1; < cNumberPlayers) {
			xSetPointer(dPlayerData, p);
			if (xGetBool(dPlayerData, xPlayerAlive)) {
				if (distanceBetweenVectors(xGetVector(dPlayerData, xPlayerPos), xGetVector(dCollectibles, xUnitPos)) < 15.0) {
					if (pickUpWeapon(p, xGetInt(dCollectibles, xCollectibleType), xGetInt(dCollectibles, xCollectibleCount))) { 
						if (xGetInt(dCollectibles, xCollectibleType) == WEAPON_KNIFE) {
							knifeCount = knifeCount - 1;
						}
						xUnitSelect(dCollectibles, xCollectibleObject);
						trUnitChangeProtoUnit("Fireball Launch Damage Effect");
						xUnitSelect(dCollectibles, xCollectiblePad);
						trUnitChangeProtoUnit("Dust Small");
						xFreeDatabaseBlock(dCollectibles);
						trQuestVarSetFromRand("sound", 1, 2, true);
						if (trCurrentPlayer() == p) {
							trSoundPlayFN("vortexland"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
						}
						break;
					}
				}
			}
		}
	}

	// WILDCARD
	switch(wildcardStep)
	{
		case 0: // waiting for someone to find the Wildcard
		{
			if (wildcard > 0) {
				trSoundPlayFN("spybirth.wav","1",-1,"","");
				trSoundPlayFN("ageadvance.wav","1",-1,"","");
				trChatSend(0, "<color={Playercolor("+wildcard+")}>{Playername("+wildcard+")}</color> has found the Wildcard!");
				overlayTextColor(wildcard);
				displayWildcardDetails();
				wildcardStep = 1;
				wildcardNext = trTimeMS() + 3000;
				prev = xGetVector(dPlayerData, xPlayerPos, wildcard);
				for(p=1; < cNumberPlayers) {
					if (p == wildcard) {
						continue;
					} else if (xGetBool(dPlayerData, xPlayerAlive, p)) {
						pos = xGetVector(dPlayerData, xPlayerPos, p);
						if (distanceBetweenVectors(pos, prev) < 900.0) {
							dir = getUnitVector(prev, pos, 50.0);
							pos = pos + dir;
							xSetPointer(dUnits, xGetInt(dPlayerData, xPlayerIndex, p));
							launchUnit(dUnits, pos);
						}
					}
				}
			}
		}
		case 1:
		{
			if (trTimeMS() > wildcardNext) {
				wildcardStep = 2;
				trMessageSetText("The treasure locations have appeared!", -1);
				if (trCurrentPlayer() == wildcard) {
					trChatSend(0, "<color=1,1,1>Your goal is to reach the treasure to earn points!</color>");
					trChatSend(0, "<color=1,1,1>Other players can now damage you and you can also damage them!</color>");
				} else {
					trChatSend(0, "<color=1,1,1>Stop the Wildcard from reaching the treasure!</color>");
					trChatSend(0, "<color=1,1,1>You can now damage the Wildcard and they can also damage you.</color>");
				}
			}
		}
		case 2:
		{
			if (xGetDatabaseCount(dGoldMines) == 0) {
				pos = xGetVector(dPlayerData, xPlayerPos, wildcard);
				prev = xsVectorSet(mapSize, 0, mapSize);
				dir = getUnitVector(prev, pos, mapSize);
				for(i=0; <3) {
					dir = rotationMatrix(dir, 0.0, 1.0);
					trQuestVarSetFromRand("rand", 0.3, 0.8, false);
					if (i == 1) {
						val = 3;
					} else {
						val = 2;
					}
					spawnGoldMine(prev + dir * trQuestVarGet("rand"), val);
				}
				trSoundPlayFN("plentybirth.wav","1",-1,"","");
			}
		}
	}

	// TREASURE
	if (xGetDatabaseCount(dGoldMines) > 0) {
		xDatabaseNext(dGoldMines);
		pos = xGetVector(dGoldMines, xUnitPos);
		if (distanceBetweenVectors(pos, xGetVector(dPlayerData, xPlayerPos, wildcard)) < 25.0) {
			earnPoints(wildcard, xGetInt(dGoldMines, xGoldMineValue));
			displayWildcardDetails();
			trSoundPlayFN("plentyvaultstolen.wav","1",-1,"","");
			trMessageSetText("The Wildcard has earned " + xGetInt(dGoldMines, xGoldMineValue) + " points!");
			removeGoldMine();
		}
	}

	// Smoke bombs
	for(i=xGetDatabaseCount(dSmokeBombs); >0) {
		xDatabaseNext(dSmokeBombs);
		prev = kbGetBlockPosition(""+xGetInt(dSmokeBombs, xUnitName), true);
		dir = xGetVector(dSmokeBombs, xProjDir);
		pos = vectorToGrid(prev + dir * 3.0);
		if ((distanceBetweenVectors(xGetVector(dSmokeBombs, xSmokeBombDest), prev) < 9.0) || terrainIsType(pos, TERRAIN_WALL, TERRAIN_WALL_SUB) || trTimeMS() > xGetInt(dSmokeBombs, xProjTimeout)) {
			prev = vectorToGrid(prev);
			val = trTimeMS() + 5000;
			for(x= -4; < 4) {
				for(y= -4; < 4) {
					if (xsPow(x, 2) + xsPow(y, 2) <= 16.0) {
						paintSmokeTile(x + xsVectorGetX(prev), y + xsVectorGetZ(prev), val);
					}
				}
			}
			xUnitSelectByID(dSmokeBombs, xUnitID);
			if (trUnitVisToPlayer()) {
				trSoundPlayFN("argusfreezeattack.wav","1",-1,"","");
				trSoundPlayFN("shockwave.wav","1",-1,"","");
			}
			for(p=1; < cNumberPlayers) {
				if (trUnitHasLOS(p)) {
					xSetPointer(dPlayerData, p);
					pos = vectorToGrid(xGetVector(dPlayerData, xPlayerPos));
					x = xsVectorGetX(pos);
					y = xsVectorGetZ(pos);
					removeFrontier(x, y, false);
					addFrontier(x, y);
					cleanFrontier();
				}
				xUnitSelectByID(dSmokeBombs, xUnitID);
			}
			trUnitChangeProtoUnit("Undermine Building Destruction SFX");
			xFreeDatabaseBlock(dSmokeBombs);
		}
	}

	// Smoke tiles
	for(i=xsMin(5, xGetDatabaseCount(dSmokeTiles)); >0) {
		xDatabaseNext(dSmokeTiles);
		x = xGetInt(dSmokeTiles, xSmokeTileX);
		y = xGetInt(dSmokeTiles, xSmokeTileY);
		if (trTimeMS() > aiPlanGetUserVariableInt(smokeArray, x, y)) {
			trPaintTerrain(x,y,x,y,xGetInt(dSmokeTiles, xSmokeTileTerrainType),xGetInt(dSmokeTiles, xSmokeTileTerrainSub),false);
			aiPlanSetUserVariableInt(smokeArray, x, y, 0);
			xFreeDatabaseBlock(dSmokeTiles);
		}
	}

	// spawn random items
	if (trTime() > nextCollectible) {
		nextCollectible = trTime() + xsPow(xsMax(0, xGetDatabaseCount(dCollectibles) - knifeCount - cNumberPlayers), 2);
		trQuestVarSetFromRand("rand", 1, mapSize, true);
		x = trQuestVarGet("rand");
		trQuestVarSetFromRand("rand", 1, mapSize, true);
		y = trQuestVarGet("rand");

		trQuestVarSetFromRand("rand", 2, WEAPON_TYPES, true);

		val = weaponQuantity(1*trQuestVarGet("rand"));
		trQuestVarSetFromRand("rand1", 1, val, true);
		trQuestVarSetFromRand("rand2", 1, val, true);
		if (trQuestVarGet("rand2") < trQuestVarGet("rand1")) {
			trQuestVarSet("rand1", trQuestVarGet("rand2"));
		}
		val = trQuestVarGet("rand1");

		spawnCollectible(perlinRoll(perlin, x, y, 1.0, 0.5), 1*trQuestVarGet("rand"), val);
	}
	
	// spawn random crates
	if (trTime() > nextCrate) {
		nextCrate = trTime() + xsPow(xsMax(0, xGetDatabaseCount(dFrostCrates) + xGetDatabaseCount(dExplosiveCrates) + 2), 2);
		trQuestVarSetFromRand("rand", 1, mapSize, true);
		x = trQuestVarGet("rand");
		trQuestVarSetFromRand("rand", 1, mapSize, true);
		y = trQuestVarGet("rand");

		trQuestVarSetFromRand("rand", dFrostCrates, dExplosiveCrates, true);
		val = trQuestVarGet("rand");

		spawnCrate(perlinRoll(perlin, x, y, 1.0, -1.0), val, getCrateProto(val), getCrateScale(val));
	}

	// frost crates
	dist = 2;
	for(i=xGetDatabaseCount(dFrostCrates); >0) {
		xDatabaseNext(dFrostCrates);
		xUnitSelectByID(dFrostCrates, xUnitID);
		if (xGetBool(dFrostCrates, xCrateActive)) {
			if (trTimeMS() > xGetInt(dFrostCrates, xCrateTimeout)) {
				for(j=xGetInt(dFrostCrates, xUnitName); < xGetInt(dFrostCrates, xCrateUnitsEnd)) {
					trUnitSelectClear();
					trUnitSelect(""+j, true);
					trUnitDestroy();
				}
				pos = xGetVector(dFrostCrates, xUnitPos);
				x = xsVectorGetX(pos);
				y = xsVectorGetZ(pos);
				for(j = -3; <= 3) {
					for(k = -3; <= 3) {
						if (j * j + k * k <= 9) {
							aiPlanSetUserVariableInt(frostArray, x + j, y + k, aiPlanGetUserVariableInt(frostArray, x + j, y + k) - 1);
						}
					}
				}
				xFreeDatabaseBlock(dFrostCrates);
			}
		} else if (trUnitPercentDamaged() > 0) {
			xSetBool(dFrostCrates, xCrateActive, true);
			xSetInt(dFrostCrates, xCrateTimeout, trTimeMS() + 5000);

			trUnitSelectClear();
			trUnitSelect(""+(1 + xGetInt(dFrostCrates, xUnitName)), true);
			trUnitChangeProtoUnit("Mist");
			trUnitSelectClear();
			trUnitSelect(""+(2 + xGetInt(dFrostCrates, xUnitName)), true);
			trUnitOverrideAnimation(2,0,true,true,-1);
			trMutateSelected(kbGetProtoUnitID("Frost Drift"));
			trSetSelectedScale(1.25 * dist, 1, 1.6 * dist);
			xUnitSelectByID(dFrostCrates, xUnitID);
			if (trUnitVisToPlayer()) {
				trSoundPlayFN("icemono.wav","1",-1,"","");
				trSoundPlayFN("siegetowerdeath.wav","1",-1,"","");
			}
			trUnitChangeProtoUnit("Fireball Launch Damage Effect");
			pos = xGetVector(dFrostCrates, xUnitPos);
			x = xsVectorGetX(pos);
			y = xsVectorGetZ(pos);
			for(j = -3; <= 3) {
				for(k = -3; <= 3) {
					if (j * j + k * k <= 9) {
						aiPlanSetUserVariableInt(frostArray, x + j, y + k, 1 + aiPlanGetUserVariableInt(frostArray, x + j, y + k));
					}
				}
			}
		}
	}

	// explosive crates
	for(i=xGetDatabaseCount(dExplosiveCrates); >0) {
		xDatabaseNext(dExplosiveCrates);
		xUnitSelectByID(dExplosiveCrates, xUnitID);
		if (trUnitPercentDamaged() > 0) {
			trUnitSelectClear();
			trUnitSelect(""+(1 + xGetInt(dExplosiveCrates, xUnitName)), true);
			trDamageUnitPercent(100);
			trUnitChangeProtoUnit("Meteorite");
			trUnitSelectClear();
			trUnitSelect(""+(2 + xGetInt(dExplosiveCrates, xUnitName)), true);
			trUnitChangeProtoUnit("Meteor Impact Ground");
			xUnitSelectByID(dExplosiveCrates, xUnitID);
			if (trUnitVisToPlayer()) {
				trSoundPlayFN("meteordustcloud.wav","1",-1,"","");
				trSoundPlayFN("siegetowerdeath.wav","1",-1,"","");
			}
			trUnitChangeProtoUnit("Fireball Launch Damage Effect");
			// shoot projectiles in all directions
			prev = gridToVector(xGetVector(dExplosiveCrates, xUnitPos));
			dir = vector(1,0,0);
			xFreeDatabaseBlock(dExplosiveCrates);

			for(j=xGetDatabaseCount(dUnits); >0) {
				xDatabaseNext(dUnits);
				if (distanceBetweenVectors(xGetVector(dUnits, xUnitPos), prev) <= 36.0) {
					trDamageUnit(3);
				}
			}

			for(j=24; >0) {
				spawnGenericProjAtPos(0, dPellets, "Petosuchus Projectile", prev, dir);
				xUnitSelectByID(dPellets, xUnitID);
				trUnitHighlight(999, false);
				pos = vectorSetAsTargetVector(prev, dir, 2.0 * mapSize * mapSize);
				trUnitMoveToPoint(xsVectorGetX(pos),0,xsVectorGetZ(pos),-1,false);
				dir = rotationMatrix(dir, 0.965926, 0.258819);
			}
		}
	}

	// pellet pos
	for(i=xGetDatabaseCount(dPellets); >0) {
		xDatabaseNext(dPellets);
		pos = kbGetBlockPosition(""+xGetInt(dPellets, xUnitName), true);
		xSetVector(dPellets, xUnitPos, pos);
		if (terrainIsType(vectorToGrid(pos), TERRAIN_WALL, TERRAIN_WALL_SUB)) {
			trQuestVarSetFromRand("sound", 1, 3, true);
			xUnitSelectByID(dPellets, xUnitID);
			if (trUnitVisToPlayer()) {
				trSoundPlayFN("mine"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
			}
			trUnitChangeProtoUnit("Lightning Sparks ground");
			xFreeDatabaseBlock(dPellets);
		}
	}

	// pellet collision detection
	hit = false;
	for(i=xGetDatabaseCount(dPellets); >0) {
		xDatabaseNext(dPellets);
		pos = xGetVector(dPellets, xUnitPos);
		prev = xGetVector(dPellets, xProjPrev);
		dist = distanceBetweenVectors(pos, prev);
		if (dist > 1.0) {
			p = xGetInt(dPellets, xUnitOwner);
			dist = xsSqrt(dist);
			dir = xGetVector(dPellets, xProjDir);
			for(j=xGetDatabaseCount(dUnits); >0) {
				xDatabaseNext(dUnits);
				if (xGetInt(dUnits, xUnitOwner) == p) {
					continue;
				} else if (targetEligible(p)) {
					if (rayCollision(dUnits, prev, dir, dist, xGetFloat(dPellets, xProjRadius))) {
						xUnitSelectByID(dUnits, xUnitID);
						trDamageUnit(1);
						xUnitSelectByID(dPellets, xUnitID);
						if (xGetInt(dUnits, xUnitOwner) > 0) {
							trQuestVarSetFromRand("sound", 1, 4, true);
							if (trUnitVisToPlayer()) {
								trSoundPlayFN("arrowonflesh"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
							}
						}
						xUnitSelectByID(dPellets, xUnitID);
						trUnitChangeProtoUnit("Lightning Sparks ground");
						xFreeDatabaseBlock(dPellets);
						hit = true;
						break;
					}
				}
			}
			if (hit == false) {
				xSetVector(dPellets, xProjPrev, pos);
			}
		}
	}

	processLaunchedUnit();

	// portals
	if (xGetDatabaseCount(dPortals) > 0) {
		xDatabaseNext(dPortals);
		for(p=1; < cNumberPlayers) {
			xSetPointer(dPlayerData, p);
			if (xGetBool(dPlayerData, xPlayerAlive) && (xGetBool(dUnits, xUnitLaunched, xGetInt(dPlayerData, xPlayerIndex)) == false)) {
				pos = xGetVector(dUnits, xUnitPos, xGetInt(dPlayerData, xPlayerIndex));
				if (distanceBetweenVectors(pos, xGetVector(dPortals, xPortalStart)) < 10.0) {
					pos = xGetVector(dPortals, xPortalEnd);
					xSetPointer(dUnits, xGetInt(dPlayerData, xPlayerIndex));
					launchUnit(dUnits, pos, true);
					trQuestVarSetFromRand("sound", 1, 3, true);
					if (trCurrentPlayer() == p) {
						inPortal = true;
					}
					xUnitSelectByID(dPlayerData, xPlayerUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("suckup"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
					}
					xUnitSelect(dPortals, xPortalUnitsEnd);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("suckup"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
					}
				}
			}
		}
		if (trTimeMS() > xGetInt(dPortals, xPortalTimeout)) {
			for(i=xGetInt(dPortals, xPortalUnitsStart); <= xGetInt(dPortals, xPortalUnitsEnd)) {
				trUnitSelectClear();
				trUnitSelect(""+i, true);
				trUnitDestroy();
			}
			xFreeDatabaseBlock(dPortals);
		}
	}

	// in portal
	if (inPortal) {
		if (xGetBool(dUnits, xUnitLaunched, xGetInt(dPlayerData, xPlayerIndex, trCurrentPlayer())) == false) {
			inPortal = false;
		} else {
			uiLookAtSelection();
		}
	}
}