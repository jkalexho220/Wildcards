const int WEAPON_KNIFE = 1;
const int WEAPON_GRAPPLING_HOOK = 2;
const int WEAPON_PORTAL = 3;
const int WEAPON_TRAP = 4;
const int WEAPON_SMOKESCREEN = 5;
const int WEAPON_SHRAPNEL_BOMB = 6;
const int WEAPON_WALL = 7;
const int WEAPON_SHIELD = 8;

const int WEAPON_TYPES = 5;

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
		case WEAPON_SHRAPNEL_BOMB:
		{
			name = "Shrapnel Bomb";
		}
		case WEAPON_WALL:
		{
			name = "Forcefield";
		}
		case WEAPON_SHIELD:
		{
			name = "Deflector Shield";
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
		case WEAPON_SMOKESCREEN:
		{
			trChatSend(0, "Blocks line of sight for 5 seconds.");
		}
		case WEAPON_TRAP:
		{
			trChatSend(0, "Arms after 1 second. Prevents the player from moving for 2 seconds.");
		}
		case WEAPON_PORTAL:
		{
			trChatSend(0, "Creates a portal through a wall.");
		}
		case WEAPON_WILDCARD:
		{
			trChatSend(0, "Whoever holds the Wildcard will be the Runner!");
		}
		case WEAPON_SHRAPNEL_BOMB:
		{
			trChatSend(0, "Explodes and releases shrapnel in all directions, dealing 1 damage.");
		}
		case WEAPON_WALL:
		{
			trChatSend(0, "Creates a barrier at your cursor (within LOS) for 5 seconds.");
			trChatSend(0, "The barrier prevents players from walking through but it does not");
			trChatSend(0, "prevent them from dashing or shooting projectiles through.");
		}
		case WEAPON_SHIELD:
		{
			trChatSend(0, "A shield blocks all projectiles that hit you for 3 seconds.");
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
			proto = kbGetProtoUnitID("Sphinx");
		}
		case WEAPON_PORTAL:
		{
			proto = kbGetProtoUnitID("Lampades");
		}
		case WEAPON_TRAP:
		{
			proto = kbGetProtoUnitID("Petrobolos");
		}
		case WEAPON_SHRAPNEL_BOMB:
		{
			proto = kbGetProtoUnitID("Fire Giant");
		}
		case WEAPON_WALL:
		{
			proto = kbGetProtoUnitID("Atlantis Wall Connector");
		}
		case WEAPON_SHIELD:
		{
			proto = kbGetProtoUnitID("Behemoth");
		}
		case WEAPON_WILDCARD:
		{
			proto = kbGetProtoUnitID("Garrison Flag Sky Passage");
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
		case WEAPON_SHRAPNEL_BOMB:
		{
			count = 2;
		}
		case WEAPON_WALL:
		{
			count = 2;
		}
		case WEAPON_SHIELD:
		{
			count = 3;
		}
		case WEAPON_WILDCARD:
		{
			count = 1;
		}
	}
	return(count);
}