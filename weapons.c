const int WEAPON_KNIFE = 1;
const int WEAPON_GRAPPLING_HOOK = 2;
const int WEAPON_PORTAL = 3;
const int WEAPON_CAMO = 4;
const int WEAPON_TRAP = 5;
const int WEAPON_SMOKE_BOMB = 6;

const int WEAPON_TYPES = 6;

const int WEAPON_WILDCARD = 99;

const int SHOOT_COOLDOWN = 500;
const int INVENTORY_SIZE = 3;

string weaponName(int weapon = 0) {
	string name = "N/A";
	switch(weapon)
	{
		case WEAPON_KNIFE:
		{
			name = "Throwing Knife";
		}
		case WEAPON_GRAPPLING_HOOK:
		{
			name = "Grappling Hook";
		}
		case WEAPON_SMOKE_BOMB:
		{
			name = "Smoke Bomb";
		}
		case WEAPON_CAMO:
		{
			name = "Camouflage";
		}
		case WEAPON_TRAP:
		{
			name = "Trap";
		}
		case WEAPON_PORTAL:
		{
			name = "Portal Gun";
		}
		case WEAPON_WILDCARD:
		{
			name = "The Wildcard";
		}
	}
	return(name);
}

void displayWeaponDetails(int weapon = 0) {
	trChatSend(0, "<color=0.9,0.9,0.9>"+weaponName(weapon)+"</color>");
	switch(weapon)
	{
		case WEAPON_KNIFE:
		{
			trChatSend(0, "Deals 1 damage and drops on the ground. Can be picked up again.");
		}
		case WEAPON_GRAPPLING_HOOK:
		{
			trChatSend(0, "Drags a player towards you or drags you towards a wall.");
		}
		case WEAPON_SMOKE_BOMB:
		{
			trChatSend(0, "Blocks line of sight for 5 seconds.");
		}
		case WEAPON_CAMO:
		{
			trChatSend(0, "Transform into a tree. Moving or attacking will reveal yourself.");
		}
		case WEAPON_TRAP:
		{
			trChatSend(0, "Deals 1 damage and stuns the player for a short duration.");
		}
		case WEAPON_PORTAL:
		{
			trChatSend(0, "Creates a portal through a wall.");
		}
		case WEAPON_WILDCARD:
		{
			trChatSend(0, "Whoever holds the Wildcard will be the Runner!");
		}
	}
}

int weaponProto(int weapon = 0) {
	int proto = kbGetProtoUnitID("Stymph Bird Feather");
	switch(weapon)
	{
		case WEAPON_GRAPPLING_HOOK:
		{
			proto = kbGetProtoUnitID("Ballista");
		}
		case WEAPON_SMOKE_BOMB:
		{
			proto = kbGetProtoUnitID("Sphinx");
		}
		case WEAPON_CAMO:
		{
			proto = kbGetProtoUnitID("Audrey");
		}
		case WEAPON_PORTAL:
		{
			proto = kbGetProtoUnitID("Lampades");
		}
		case WEAPON_TRAP:
		{
			proto = kbGetProtoUnitID("Petrobolos");
		}
		case WEAPON_WILDCARD:
		{
			proto = kbGetProtoUnitID("Garrison Flag Sky Passage");
		}
	}
	return(proto);
}