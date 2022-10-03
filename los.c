/*
Visible = first bit
Frontier = second bit

X = start unit name
Y = array of ints
*/
int losDimension = 0;

const int LOS_VISIBLE = 1;
const int LOS_FRONTIER = 2;
const int LOS_FRONTIER_AND_VISIBLE = 3;

const float LOS_RADIUS = 150.0;

/*
Accepts the x,y grid coordinates and performs ray tracing.
Stops when it hits TERRAIN_WALL
*/
bool rayTrace(int fromX = 0, int fromY = 0, int toX = 0, int toY = 0) {
	vector pos = xsVectorSet(fromX, 0, fromY);
	vector step = xsVectorSet(toX, 0, toY) - pos;
	int count = xsVectorLength(step);
	step = xsVectorNormalize(step);
	int x = fromX;
	int y = fromY;
	float diff = 0;
	for(i=0; < count) {
		pos = pos + step;
		diff = xsVectorGetX(pos) - x;
		if (xsAbs(diff) > 0.5) {
			if (diff > 0) {
				x = x + 1;
			} else {
				x = x - 1;
			}
		}
		diff = xsVectorGetZ(pos) - y;
		if (xsAbs(diff) > 0.5) {
			if (diff > 0) {
				y = y + 1;
			} else {
				y = y - 1;
			}
		}
		if (trGetTerrainType(x, y) == TERRAIN_WALL) { // all cliffs are terrain primary 2, so we don't need to check secondary
			return(false);
		}
	}
	return(true);
}

bool nodeInFrontier(int data = 0) {
	return(data / 2 == 1);
}

bool nodeIsVisible(int data = 0) {
	return(iModulo(2, data) == 1);
}

bool coordinatesInMap(int x = 0, int y = 0) {
	return(x >= 0 && y >= 0 && x < losDimension && y < losDimension);
}

vector losGridToPos(int x = 0, int y = 0) {
	return(xsVectorSet(x * 4, 0, y * 4));
}

float distanceBetweenCoordinates(int fromX = 0, int fromY = 0, int toX = 0, int toY = 0) {
	float diffX = xsPow(toX - fromX, 2);
	float diffY = xsPow(toY - fromY, 2);
	return(diffX + diffY);
}

float distanceManhattan(vector from = vector(0,0,0), vector to = vector(0,0,0)) {
	return(xsAbs(xsVectorGetX(from) - xsVectorGetX(to)) + xsAbs(xsVectorGetZ(from) - xsVectorGetZ(to)));
}

int losGridToIndex(int x = 0, int y = 0) {
	return(x * losDimension + y);
}

void addToFrontier(int frontier = 0, int x = 0, int y = 0) {
	int db = xGetInt(dPlayerData, xPlayerLosDB);
	zSetInt(db, losGridToIndex(x, y), LOS_FRONTIER + zGetInt(db, losGridToIndex(x, y)));
	xAddDatabaseBlock(frontier);
	xSetInt(frontier, xFrontierX, x, xGetNewestPointer(frontier));
	xSetInt(frontier, xFrontierY, y, xGetNewestPointer(frontier));
}

/* 
assume dPlayerData pointer is correct
posX and posY are grid coordinates
*/
void removeFrontier(int posX = 0, int posY = 0) {
	int frontier = xGetInt(dPlayerData, xPlayerLosFrontier);
	int db = xGetInt(dPlayerData, xPlayerLosDB);
	int start = xGetInt(dPlayerData, xPlayerLosStart);
	int end = xGetPrev(frontier);
	int current = 0;
	int index = 0;
	int data = 0;
	int x = 0;
	int y = 0;
	int newX = 0;
	int newY = 0;
	while(xGetDatabaseCount(frontier) > 0) {
		current = xDatabaseNext(frontier);
		x = xGetInt(frontier, xFrontierX);
		y = xGetInt(frontier, xFrontierY);
		index = losGridToIndex(x, y);
		data = zGetInt(db, index);
		if (rayTrace(posX, posY, x * 2, y * 2) == false || distanceBetweenCoordinates(posX, posY, x * 2, y * 2) > LOS_RADIUS) {
			index = losGridToIndex(x, y);
			zSetInt(db, index, 0);
			xFreeDatabaseBlock(frontier);
			trUnitSelectClear();
			trUnitSelectByID(start + index);
			trMutateSelected(kbGetProtoUnitID("Cinematic Block"));
			for(i= -1; <= 1) {
				for(j= -1; <= 1) {
					if (xsAbs(i) + xsAbs(j) == 0) {
						continue;
					} else {
						newX = x + i; 
						newY = y + j;
						if (coordinatesInMap(newX, newY)) {
							index = losGridToIndex(newX, newY);
							data = zGetInt(db, index);
							if (data == LOS_VISIBLE) {
								addToFrontier(frontier, newX, newY);
							}
						}
					}
				}
			}
		}
		if (current == end) {
			break;
		}
	}
}

/*
assume dPlayerData pointer is correct
posX and posY are grid coordinates
*/
void addFrontier(int posX = 0, int posY = 0) {
	int frontier = xGetInt(dPlayerData, xPlayerLosFrontier);
	int db = xGetInt(dPlayerData, xPlayerLosDB);
	int start = xGetInt(dPlayerData, xPlayerLosStart);
	int end = xGetPrev(frontier);
	int current = 0;
	int index = 0;
	int data = 0;
	int x = 0;
	int y = 0;
	int newX = 0;
	int newY = 0;
	bool firstPass = xGetDatabaseCount(frontier) == 0;
	if (firstPass) {
		addToFrontier(frontier, posX / 2, posY / 2);
	}
	for(k=1*LOS_RADIUS; >0) {
		if (xGetDatabaseCount(frontier) == 0) {
			break;
		}
		current = xDatabaseNext(frontier);
		x = xGetInt(frontier, xFrontierX);
		y = xGetInt(frontier, xFrontierY);
		index = losGridToIndex(x, y);
		data = zGetInt(db, index);
		if (nodeIsVisible(data) == false) {
			if (trGetTerrainType(x * 2, y * 2) != TERRAIN_WALL) {
				if (distanceBetweenCoordinates(x * 2, y * 2, posX, posY) < LOS_RADIUS) {
					if (rayTrace(posX, posY, x * 2, y * 2)) {
						data = LOS_FRONTIER_AND_VISIBLE; // node is visible and in the frontier
						zSetInt(db, index, data);
						trUnitSelectClear();
						trUnitSelectByID(start + index);
						trMutateSelected(kbGetProtoUnitID("Revealer to Player"));
						//trUnitChangeProtoUnit("Revealer to Player");
					}
				}
			}
		}
		if (nodeIsVisible(data)) {
			for(i= -1; <= 1) {
				for(j= -1; <= 1) {
					if (xsAbs(i) + xsAbs(j) == 0) {
						continue;
					} else {
						newX = x + i;
						newY = y + j;
						if (coordinatesInMap(newX, newY)) {
							index = losGridToIndex(newX, newY);
							if (zGetInt(db, index) == 0) { // node is not visible and not in the frontier
								addToFrontier(frontier, newX, newY);
							}
						}
					}
				}
			}
		}
		if (current == end) {
			if (firstPass) {
				firstPass = false;
			} else {
				break;
			}
		}
	}
}

void cleanFrontier() {
	int frontier = xGetInt(dPlayerData, xPlayerLosFrontier);
	int db = xGetInt(dPlayerData, xPlayerLosDB);
	int start = xGetInt(dPlayerData, xPlayerLosStart);
	bool safe = false;
	int index = 0;
	int data = 0;
	int x = 0;
	int y = 0;
	int newX = 0;
	int newY = 0;

	for(k=xGetDatabaseCount(frontier); >0) {
		xDatabaseNext(frontier);
		x = xGetInt(frontier, xFrontierX);
		y = xGetInt(frontier, xFrontierY);
		index = losGridToIndex(x, y);
		data = zGetInt(db, index);
		if (nodeIsVisible(data) == false) {
			// something can be in the database but not visible
			zSetInt(db, index, 0);
			xFreeDatabaseBlock(frontier);
		} else {
			safe = false;
			for(i= -1; <= 1) {
				for(j= -1; <= 1) {
					if (xsAbs(i) + xsAbs(j) == 0) {
						continue;
					} else {
						newX = x + i;
						newY = y + j;
						if (coordinatesInMap(newX, newY)) {
							if (nodeIsVisible(zGetInt(db, losGridToIndex(newX, newY))) == false) {
								safe = true;
								break;
							}
						}
					}
				}
				if (safe) {
					break;
				}
			}
			if (safe == false) {
				zSetInt(db, index, LOS_VISIBLE);
				xFreeDatabaseBlock(frontier);
			}
		}
	}
	/*
	for(i=0; < losDimension * losDimension) {
		if (nodeIsVisible(zGetInt(db, i))) {
			trUnitSelectClear();
			trUnitSelectByID(start + i);
			trMutateSelected(kbGetProtoUnitID("Lampades Bolt"));
		}
	}
	for(i=xGetDatabaseCount(frontier); >0) {
		xDatabaseNext(frontier);
		x = xGetInt(frontier, xFrontierX);
		y = xGetInt(frontier, xFrontierY);
		trUnitSelectClear();
		trUnitSelectByID(start + losGridToIndex(x, y));
		trMutateSelected(kbGetProtoUnitID("Dwarf"));
	}
	*/
}

void modifyLOS(string proto = "", int p = 0) {
	// HP
	trModifyProtounit(proto, p, 0, 9999999999999999999.0);
	trModifyProtounit(proto, p, 0, -9999999999999999999.0);
	trModifyProtounit(proto, p, 0, 3.0);
	// speed
	trModifyProtounit(proto, p, 1, 9999999999999999999.0);
	trModifyProtounit(proto, p, 1, -9999999999999999999.0);
	trModifyProtounit(proto, p, 1, 5.0);
	// LOS
	trModifyProtounit(proto, p, 2, 9999999999999999999.0);
	trModifyProtounit(proto, p, 2, -9999999999999999999.0);
	trModifyProtounit(proto, p, 2, 3.0);
}

rule setup_los
inactive
highFrequency
{
	xsDisableSelf();
	trSetUnitIdleProcessing(false);

	gadgetUnreal("ScoreDisplay");
	gadgetUnreal("GodPowers");
	gadgetUnreal("tributedlg-sendButton");
	gadgetUnreal("tributedlg-clearButton");
	%
	for(p=1; < cNumberNonGaiaPlayers) {
		code("trStringQuestVarSet(\"p"+p+"name\", \""+rmGetPlayerName(p)+"\");");
	}
	%
	
	losDimension = mapSize / 2;
	int dbName = 0;
	int loc = 0;
	int next = 0;
	for(p=1; < cNumberPlayers) {
		modifyLOS("Hoplite", p);
		modifyLOS("Pharaoh of Osiris", p);
		modifyLOS("Hero Greek Odysseus", p);
		modifyLOS("Minotaur", p);
		modifyLOS("Hero Greek Jason", p);
		modifyLOS("Female", p);
		trModifyProtounit("Revealer to Player", p, 2, 9999999999999999999.0);
		trModifyProtounit("Revealer to Player", p, 2, -9999999999999999999.0);
		trModifyProtounit("Revealer to Player", p, 2, 3.0);
		dbName = zNewArray(mInt, losDimension * losDimension, "playerLOS"+p);
		xSetPointer(dPlayerData, p);
		xSetInt(dPlayerData, xPlayerLosStart, trGetNextUnitScenarioNameNumber());
		xSetInt(dPlayerData, xPlayerLosDB, dbName);
		loc = 0;
		for(x=0; < losDimension) {
			for(y=0; < losDimension) {
				zSetInt(dbName, loc + y, 0);
				next = trGetNextUnitScenarioNameNumber();
				trArmyDispatch(""+p+",0","Revealer to Player",1,1 + x * 4, 0, 1 + y * 4, 0, true);
				trUnitSelectClear();
				trUnitSelect(""+next, true);
				trMutateSelected(kbGetProtoUnitID("Cinematic Block"));
			}
			loc = loc + losDimension;
		}
		xSetInt(dPlayerData, xPlayerLosStart, kbGetBlockID(""+xGetInt(dPlayerData, xPlayerLosStart)));
		trArmyDispatch(""+p+",0","Victory Marker",1,1,0,1,0,true);
	}

	if (aiIsMultiplayer()) {
		//  data load stuff
		for(i=0; < 32 * (cNumberPlayers - 1)) {
			trUnitSelectClear();
			trUnitSelectByID(i);
			trUnitDestroy();
		}
	}

	xsEnableRule("Z_cin_00");
}

rule track_los
inactive
highFrequency
{
	for(p=1; < cNumberPlayers) {
		xSetPointer(dPlayerData, p);
		if (xGetBool(dPlayerData, xPlayerAlive)) {
			vector pos = kbGetBlockPosition(""+xGetInt(dPlayerData, xPlayerUnitName), true);
			if (distanceManhattan(pos, xGetVector(dPlayerData, xPlayerPos)) >= 2.0) {
				int posX = xsVectorGetX(pos) / 2;
				int posY = xsVectorGetZ(pos) / 2;
				removeFrontier(posX, posY);
				addFrontier(posX, posY);
				cleanFrontier();
				xSetVector(dPlayerData, xPlayerPos, pos);
			}
		}
	}
}