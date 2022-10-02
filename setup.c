

rule terrainRandom
active
highFrequency
{
	int perlin = generatePerlinNoise(100, 10);
	for(x=0; <= 100) {
		for(y=0; <= 100) {
			trChangeTerrainHeight(x, y, x, y, -10.0 + 10.0 * getPerlinNoise(perlin, x, y));
		}
	}
	trSetFogAndBlackmap(false, false);
	xsDisableSelf();
}