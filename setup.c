%
code("int mapSize = " + 1*sqrt(cNumberNonGaiaPlayers * 1200) + ";");
%

int TERRAIN_WALL_PRIMARY = 2;
int TERRAIN_WALL_SECONDARY = 0;

rule immediate
active
highFrequency
runImmediately
{
	xsDisableSelf();
	trSetUnitIdleProcessing(false);

	aiSet("NoAI", 0);
	for(p=1; < cNumberPlayers) {
		for(x=cNumberPlayers - 1; >p) {
			trPlayerSetDiplomacy(p, x, "neutral");
			trPlayerSetDiplomacy(x, p, "neutral");
		}
		trPlayerSetDiplomacy(0, p, "Enemy");
		trSetCivAndCulture(p, 1, 0);
	}
	trTechSetStatus(0, 304, 4); // omniscience p0
}