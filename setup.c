%
code("int mapSize = " + 1*sqrt(cNumberNonGaiaPlayers * 2000) + ";");
%

int TERRAIN_WALL_PRIMARY = 2;
int TERRAIN_WALL_SECONDARY = 0;

rule terrainRandom
active
highFrequency
{
	trChangeTerrainHeight(0,0,mapSize+1,mapSize+1,0,false);
	int perlin = generatePerlinNoise(mapSize, 10);
	float noise = 0;
	for(x=0; <= mapSize) {
		for(y=0; <= mapSize) {
			noise = getPerlinNoise(perlin, x, y);
			if (noise > 1.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_WALL_PRIMARY,TERRAIN_WALL_SECONDARY,false);
				trChangeTerrainHeight(x, y, x, y, noise);
			}
		}
	}
	trSetFogAndBlackmap(false, false);
	xsDisableSelf();
}