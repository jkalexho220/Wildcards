const int WEAPON_KNIFE = 1;
const int WEAPON_GRAPPLING_HOOK = 2;
const int WEAPON_PORTAL = 3;
const int WEAPON_TRAP = 4;
const int WEAPON_SMOKESCREEN = 5;
const int WEAPON_SWORD = 6;
const int WEAPON_SNIPER = 7;
const int WEAPON_CAMERA = 8;

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
		case WEAPON_SMOKESCREEN:
		{
			name = "Smokescreen";
		}
		case WEAPON_TRAP:
		{
			name = "Trap";
		}
		case WEAPON_PORTAL:
		{
			name = "Portal Gun";
		}
		case WEAPON_CAMERA:
		{
			name = "Obelisk";
		}
		case WEAPON_SNIPER:
		{
			name = "Longbow";
		}
		case WEAPON_WILDCARD:
		{
			name = "The Wildcard";
		}
		case WEAPON_SWORD:
		{
			name = "Whirlwind";
		}
	}
	return(name);
}

void displayWeaponDetails(int weapon = 0) {
	trChatSend(0, "<color=0.9,0.9,0.9><u>"+weaponName(weapon)+"</u></color>");
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
		case WEAPON_SMOKESCREEN:
		{
			trChatSend(0, "Blocks line of sight for 5 seconds.");
		}
		case WEAPON_TRAP:
		{
			trChatSend(0, "Arms after 2 seconds. Snares an enemy player for 2 seconds.");
			trChatSend(0, "You can step on your own traps without triggering them.");
		}
		case WEAPON_PORTAL:
		{
			trChatSend(0, "Creates a one-way portal through a wall for 30 seconds.");
			trChatSend(0, "Anyone can use the portal.");
		}
		case WEAPON_WILDCARD:
		{
			trChatSend(0, "Whoever holds the Wildcard will be the Runner!");
		}
		case WEAPON_SNIPER:
		{
			trChatSend(0, "Shoots a fast arrow that passes through walls and deals 2 damage.");
		}
		case WEAPON_CAMERA:
		{
			trChatSend(0, "Gives vision in a circle that ignores walls. Can be destroyed.");
		}
		case WEAPON_SWORD:
		{
			trChatSend(0, "Deals 1 damage in a circle around you and then blocks knives for 1 second.");
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
		case WEAPON_SMOKESCREEN:
		{
			proto = kbGetProtoUnitID("Terracotta Soldier");
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
		case WEAPON_CAMERA:
		{
			proto = kbGetProtoUnitID("Outpost");
		}
		case WEAPON_SNIPER:
		{
			proto = kbGetProtoUnitID("Archer Atlantean");
		}
		case WEAPON_SWORD:
		{
			proto = kbGetProtoUnitID("Sphinx");
		}
	}
	return(proto);
}

int weaponQuantity(int weapon = 0) {
	int count = 1;
	switch(weapon)
	{
		case WEAPON_GRAPPLING_HOOK:
		{
			count = 2;
		}
		case WEAPON_SMOKESCREEN:
		{
			count = 2;
		}
		case WEAPON_PORTAL:
		{
			count = 1;
		}
		case WEAPON_TRAP:
		{
			count = 4;
		}
		case WEAPON_WILDCARD:
		{
			count = 1;
		}
		case WEAPON_CAMERA:
		{
			count = 1;
		}
		case WEAPON_SNIPER:
		{
			count = 3;
		}
		case WEAPON_SWORD:
		{
			count = 3;
		}
	}
	return(count);
}