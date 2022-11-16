rule skip_cinematic
inactive
highFrequency
{
	trUnitSelectClear();
	trUnitSelectByQV("skipButton");
	if (trUnitAlive() == false) {
		xsDisableSelf();
		cinNext = 0;
	}
}

rule skip_cinematic_2
inactive
highFrequency
{
	if (trTime() > cActivationTime + 2) {
		trDelayedRuleActivation("gameplay_start");
		trUIFadeToColor(0,0,0,1000,0,false);
		trLetterBox(false);
		xsDisableSelf();
	}
}


rule intro_cinematic
inactive
highFrequency
{
	if (trTime() > cinNext) {
		vector dir = vector(0,0,0);
		vector pos = vector(0,0,0);
		switch(cinStep)
		{
			case 0:
			{
				trFadeOutAllSounds(1.0);
				trForceNonCinematicModels(true);
				trLetterBox(true);
				trUIFadeToColor(0,0,0,900,0,true);
				cinNext = trTime() + 1;
			}
			case 1:
			{
				xsEnableRule("build_map");
				xsDisableRule("skip_cinematic");
				trUnitSelectClear();
				trUnitSelectByQV("skipButton");
				if (trUnitAlive() == false) {
					xsDisableSelf();
					xsEnableRule("skip_cinematic_2");
				} else {
					trUnitDestroy();
					cinNext = trTime() + 1;
				}
			}
			case 2:
			{
				trUIFadeToColor(0,0,0,1000,0,false);
				lookAt(xsVectorGetX(startPos), xsVectorGetZ(startPos));
				trackInsert();
				trackPlay(6000, -1);
				trCameraCut(vector(-35.355,50,-35.355) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				trackAddWaypoint();
				trCameraCut(xsVectorSet(xsVectorGetX(startPos) - 50,50,xsVectorGetZ(startPos)), vector(0.7071,-0.7071,0), vector(0.7071,0.7071,0), vector(0,0,-1));
				trackAddWaypoint();
				trackPlay(6000, -1);
				trSoundPlayFN("music\interface\gank sneakin'.mp3", "1", -1, "Zenophobia:Welcome to Wildcards! This is a game of speed and trickery!", "icons\infantry g hoplite icon 64");
				cinNext = trTime() + 6;
			}
			case 3:
			{
				trSoundPlayFN("","1",-1,"Zenophobia:In this game, one player is the Wildcard, and their goal is to steal treasure.", "icons\infantry g hoplite icon 64");
				cinNext = trTime() + 7;
				trQuestVarSetFromRand("rand", 0, 6.283185, false);
				dir = vectorSetFromAngle(trQuestVarGet("rand"));
				wildcard = 1;
				pos = spawnGoldMine(xsVectorSet(mapSize, 0, mapSize) + dir * mapSize * 0.5, 3);
				lookAt(xsVectorGetX(pos), xsVectorGetZ(pos));
				trackInsert();
				trackPlay(7000, -1);
				trCameraCut(vector(-35.355,50,-35.355) + pos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				trackAddWaypoint();
				trCameraCut(xsVectorSet(xsVectorGetX(pos),50,xsVectorGetZ(pos) - 50), vector(0,-0.7071,0.7071), vector(0,0.7071,0.7071), vector(1,0,0));
				trackAddWaypoint();
				trackPlay(7000, -1);
			}
			case 4:
			{
				trSoundPlayFN("","1",-1,"Zenophobia:The other players will try to stop the Wildcard with any means necessary.", "icons\infantry g hoplite icon 64");
				cinNext = trTime() + 7;
				trCameraCut(vector(-46.464447,70.710701,-46.464447) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				xDatabaseNext(dGoldMines);
				removeGoldMine();
				wildcard = 0;
			}
			case 5:
			{
				trSoundPlayFN("","1",-1,"Zenophobia:However, you cannot damage other players normally. Instead, you must use items scattered across the map.", "icons\infantry g hoplite icon 64");
				cinNext = trTime() + 8;
				pos = xsVectorSet(mapSize / 2, 0, mapSize / 2);
				trCameraCut(vector(-46.464447,70.710701,-46.464447) + pos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				trackInsert();
				trackPlay(8000, -1);
				pos = xsVectorSet(mapSize * 3 / 2, 0, mapSize / 2);
				trCameraCut(vector(-46.464447,70.710701,-46.464447) + pos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				trackAddWaypoint();
				trackPlay(8000, -1);
			}
			case 6:
			{
				trSoundPlayFN("","1",-1,"Zenophobia:If the Wildcard dies, they will drop a Spotlight, and whoever grabs it becomes the new Wildcard!","icons\infantry g hoplite icon 64");
				dir = vectorSetFromAngle(trQuestVarGet("rand"));
				pos = xsVectorSet(mapSize / 2, 0, mapSize / 2) - dir * mapSize / 4;
				pos = perlinRoll(perlin, xsVectorGetX(pos), xsVectorGetZ(pos));
				spawnCollectible(pos, WEAPON_WILDCARD, 1);
				trCameraCut(vector(-8.485,12,-8.485) * 2.0 + pos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				cinNext = trTime() + 8;
			}
			case 7:
			{
				xDatabaseNext(dCollectibles);
				xUnitSelect(dCollectibles, xCollectibleObject);
				trUnitDestroy();
				xUnitSelect(dCollectibles, xCollectiblePad);
				trUnitDestroy();
				xFreeDatabaseBlock(dCollectibles);
				cinNext = trTime() + 8;
				trSoundPlayFN("","1",-1,"Zenophobia:The first player to reach " + victoryCount + " points wins! Good luck and have fun!","icons\infantry g hoplite icon 64");
				trCameraCut(vector(-9.292889,14.14214,-9.292889) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				trackInsert();
				trackPlay(7000, -1);
				trCameraCut(vector(-46.464447,70.710701,-46.464447) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
				trackAddWaypoint();
				trackPlay(7000, -1);
				trFadeOutAllSounds(6);
			}
			case 8:
			{
				trLetterBox(false);
				xsEnableRule("gameplay_start");
				xsDisableSelf();
				trChatSend(0, "Type 'help' to view additional information.");
				xsEnableRule("help_me");
			}
		}
		cinStep = cinStep + 1;
	}
}


rule help_me
inactive
highFrequency
{
	if (trChatHistoryContains("help", trCurrentPlayer())) {
		xsDisableSelf();
		trChatSend(0, "<color=1,1,1><u>Rules</u></color>");
		trChatSend(0, "First to 20 points wins. The Wildcard scores points by capturing treasure.");
		trChatSend(0, "If the Wildcard dies, they drop a Spotlight. Touch it to become the new Wildcard.");
		trChatSend(0, "Knives deal 1 damage and can be intercepted by other knives.");
		trChatSend(0, "Your dash deals 1 damage if it hits an enemy.");
	}
}