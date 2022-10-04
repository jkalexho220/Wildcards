%
code("int mapSize = " + 1*sqrt(MAP_SIZE_STATIC + cNumberNonGaiaPlayers * MAP_SIZE_DYNAMIC) + ";");
%

int TERRAIN_WALL = 2;
int TERRAIN_WALL_SUB = 1;

int TERRAIN_WALL_BASE = 0;
int TERRAIN_WALL_BASE_SUB = 5;

int TERRAIN_PRIMARY = 0;
int TERRAIN_PRIMARY_SUB = 2;

int TERRAIN_DIRT = 0;
int TERRAIN_DIRT_SUB = 3;

int TERRAIN_ROAD = 0;
int TERRAIN_ROAD_SUB = 65;


rule immediate
active
highFrequency
runImmediately
{
	xsDisableSelf();
	trSetUnitIdleProcessing(false);

	gadgetUnreal("ScoreDisplay");
	gadgetUnreal("GodPowers");
	gadgetUnreal("tributedlg-sendButton");
	gadgetUnreal("tributedlg-clearButton");

	trUIFadeToColor(0,0,0,0,0,true);
	trLetterBox(true);

	configUndef("ErodeBuildingFoundations");

	aiSet("NoAI", 0);
	for(p=1; < cNumberPlayers) {
		for(x=cNumberPlayers - 1; >p) {
			trPlayerSetDiplomacy(p, x, "neutral");
			trPlayerSetDiplomacy(x, p, "neutral");
		}
		trPlayerSetDiplomacy(0, p, "Enemy");
		trSetCivAndCulture(p, 1, 0);

		trPlayerKillAllGodPowers(p);
		trPlayerTechTreeEnabledGodPowers(p, false);

		trPlayerGrantResources(p, "Food", -999);
		trPlayerGrantResources(p, "Wood", -999);
		trPlayerGrantResources(p, "Gold", -999);
		trPlayerGrantResources(p, "Favor", -999);

		trModifyProtounit("Dwarf",p,55,4);
	}

	
	trModifyProtounit("Phoenix Egg",1,55,4);
	trTechSetStatus(0, 304, 4); // omniscience p0
}

void modifyPlayableProto(string proto = "", int p = 0) {
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
	// armor
	trModifyProtounit(proto, p, 24, -1);
	trModifyProtounit(proto, p, 25, -1);
	trModifyProtounit(proto, p, 26, -1);

}

void modifyBuildableProto(string proto = "", int p = 0) {
	trModifyProtounit(proto, p, 55, 4); // flying
	// LOS
	trModifyProtounit(proto, p, 2, 9999999999999999999.0);
	trModifyProtounit(proto, p, 2, -9999999999999999999.0);
	trModifyProtounit(proto, p, 2, 0.0);
	// cost gold/wood/food/favor
	for(i=16; <= 19) {
		trModifyProtounit(proto, p, i, 9999999999999999999.0);
		trModifyProtounit(proto, p, i, -9999999999999999999.0);
		trModifyProtounit(proto, p, i, 0.0);
	}
	// build points
	trModifyProtounit(proto, p, 4, -999);
}

rule delayed_modify
inactive
highFrequency
{
	xsDisableSelf();
	for(p=1; < cNumberPlayers) {
		modifyPlayableProto("Peltast", p);
		modifyPlayableProto("Hoplite", p);
		modifyPlayableProto("Pharaoh of Osiris", p);
		modifyPlayableProto("Hero Greek Odysseus", p);
		modifyPlayableProto("Minotaur", p);
		modifyPlayableProto("Hero Greek Jason", p);
		modifyPlayableProto("Female", p);

		modifyBuildableProto("House", p);
		trModifyProtounit("House", p, 18, -1); // house grants 1 food
		modifyBuildableProto("Granary", p);
		trModifyProtounit("Granary", p, 17, -1); // tower grants 1 wood
		modifyBuildableProto("Storehouse", p);
		trModifyProtounit("Storehouse", p, 16, -1); // storehouse grants 1 gold

		trModifyProtounit("Stymph Bird Feather", p, 1, 9999999999999999999.0);
		trModifyProtounit("Stymph Bird Feather", p, 1, -9999999999999999999.0);
		trModifyProtounit("Stymph Bird Feather", p, 1, 20.0);
	}
}


rule Z_cin_00
inactive
highFrequency
{
	if (trTime() > cActivationTime + 1) {
		trSetFogAndBlackmap(false, false);
		/* logo */
		trCameraCut(vector(25,90,27), vector(0,-1,0), vector(0,0,1), vector(1,0,0));
		int posX = 25 - 10;
		int posZ = 25 + 10;
		trModifyProtounit("Curse SFX", 1, 8, 8);
		trArmyDispatch("1,0", "Dwarf",1,posX,0,posZ,0,true);
		for(x=10; >0) {
			posX = posX + 2;
			trArmyDispatch("1,0", "Dwarf",1,posX,0,posZ,0,false);
		}
		for(x=5; >0) {
			posX = posX - 2;
			posZ = posZ - 2;
			trArmyDispatch("1,0", "Dwarf",1,posX,0,posZ,0,false);
		}
		for (x=4; >0) {
			posX = posX + 2;
			posZ = posZ - 2;
			trArmyDispatch("1,0", "Dwarf",1,posX,0,posZ,0,false);
		}
		trArmySelect("1,0");
		trUnitChangeProtoUnit("Curse SFX");
		
		trUIFadeToColor(0,0,0,500,0,false);
		trSoundPlayFN("cinematics\15_in\gong.wav", "4", -1, "","");
		trSoundPlayFN("godpower.wav", "6", -1, "","");
		trOverlayText("Zenophobia presents...", 4.0, -1, -1, -1);
		
		xsEnableRule("Z_cin_01");
		xsDisableSelf();
	}
}

rule Z_cin_01
inactive
highFrequency
{
	if (trTime() > cActivationTime + 5) {
		trSoundPlayFN("spybirth.wav","1",-1,"","");
		trSoundPlayFN("specialist.wav","1",-1,"","");
		trOverlayText("Wildcards", 3.0, -1, -1, -1);
		trUIFadeToColor(0,0,0,1000,3000,true);
		xsEnableRule("Z_cin_02");
		xsDisableSelf();
	}
}


rule Z_cin_02
inactive
highFrequency
{
	int next = 0;
	if (trTime() > cActivationTime + 5) {
		trModifyProtounit("Curse SFX", 1, 8, -8);
		
		trCameraCut(vector(145,70,26), vector(0,-0.7071,0.7071), vector(0,0.7071,0.7071), vector(1,0,0));
		trLetterBox(false);

		trUIFadeToColor(0,0,0,0,0,false);

		xsEnableRule("build_map");
		xsEnableRule("delayed_modify");

		trSetLighting("default", 0.1);
		xsDisableSelf();
	}
}
