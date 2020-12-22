#pragma once

//int
static int Tabs = 0;
static int SubTabLegit = 0;
static int SubTabMisc = 0;
static int SubTabSkins = 0;
static int SubTabVisuals = 0;

//char
const char* chams_material[] = { "Regular", "Flat", "Glow", "Glow Armsrace" };
const char* esp_boxes[] = { "Normal", "Edge"};

const char* player_model_t[] = { "Default",
								//leet
								"The Elite Mr. Muhlik", "Prof. Shahmat", "Osiris", "Ground Rebel",
								//balkan
								"Maximus", "Dragomir", "'the Doctor' Romanov", "Rezan The Ready",
								"Blackwolf", "Rezan The Redshirt", "Dragomir Footsoldier",
								//phoenix
								"Soldier", "Enforcer", "Slingshot", "Street Soldier",
								//proffesional
								"Sir Bloody Miami Darryl", "Sir Bloody Silent Darryl", "Sir Bloody Skullhead Darryl",
								"Sir Bloody Darryl Royale", "Sir Bloody Loudmouth Darryl", "Safecracker Voltzmann",
								"Little Kev", "Number K", "Getaway Sally" };

const char* player_model_ct[] = { "Default",
								//fbi 
								"Special Agent Ava", "Michael Syfers", "Markus Delrow", "Operator",
								//st6
								"Seal Team 6 Soldier", "'two Times' Mccoy", "Buckshot", "3rd Commando Company",
								"Lt. Commander Ricksaw", "'blueberries' Buckshot", "'two Times' Mccoy ",
								//swat
								"Cmdr. Mae 'dead Cold' Jamison", "1st Lieutenant Farlow", "John 'van Healen' Kask",
								"Bio-haz Specialist", "Sergeant Bombson", "Chem-haz Specialist",
								//sas
								"B Squadron Officer" };

static std::string sub_legit[] = { "Aimbot", "Trigger" };
static std::string sub_visuals[] = { "Esp", "Glow", "Chams", "Misc", "Colors" };
static std::string sub_skins[] = { "Skins", "Inventory", "Profile" };
static std::string sub_misc[] = { "Misc", "Config" };
static std::string tabs[] = { "Legit", "Visuals", "Skins", "Misc" };