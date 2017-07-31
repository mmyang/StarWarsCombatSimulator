#include "stdafx.h"

#include "VehicleDatabase.h"

void scenario_notes() {
  // Using stats from Fantasy Flight Star Wars Age of Rebellion where available.
  // Unofficial stats: http://bastionkainssweote.blogspot.com/p/shipyard.html
  // Vulture droid: https://community.fantasyflightgames.com/topic/92511-starfighter-creation-thread/?page=2
  // More starships: https://jegergryte.wordpress.com/starwarsrpg/the-edge-of-the-empire-supplements/
  // More starships: http://swrpg.viluppo.net/transportation/starships/

  // Each squadron (12) is three flights of four fighters.
  // An ISD carries a wing (72) of 6 squadrons.
  // A Vindicator carries 24 fighters.
  // Dreadnought carries 12 fighters.
  // TIE:            50,000 CR
  // Interceptor:    75,000 CR
  // X-Wing:        120,000 CR (5 X-Wings == 12 TIEs)
  // Corvette:    1,200,000 CR (24 TIEs || 10 X-Wings)
  // Arquitens:   2,000,000 CR
  // Dreadnought: 7,200,000 CR
  // Vindicator: 10,400,000 CR
  // ISD:       150,000,000 CR

  // ? Out of date: Forgot to give TIEs 2 armor, was 0
  // 2 TIEs beat 1 X-Wing 70%
  // 2 TIEs beat 1 AT-ST 75%
  // 1 X-Wing beat 1 AT-ST 80%
  // 6 TIE vs 4 X-Wing
  // vs Corvette, 9 TIE ~= 4 X-Wing
  // 9 X-Wing vs Dreadnought
  // 11 X-Wing vs Vindicator
  // 32 X-Wing vs Vindicator + 24 TIE ?
  // 29 X-Wing vs ISD
  // 78 X-Wing vs ISD + 24 TIE ?
  // 138 TIE vs 78 X-Wing ?
  // 3 Dread beat 1 ISD 3%
  // 1 Dread beat 1 Vindicator 70%
  // 1 Dread + 12 X-Wing always lose to 1 Vindicator + 24 TIE
  // 3 Dread + 36 X-Wing beat 1 ISD + 46 TIE 22%

  // Difference between TIE and Interceptor not apparent at 3 Green skill
}

VehicleDatabase::VehicleDatabase() {
  // TODO: Add AT-ST, AT-AT, speeder bikes to VehicleDatabase

  {
    // Vulture droids are starfighter-sized and remotely commanded in swarms.
    map_name_weapon["blaster_cannons_vultr"] = Weapon(5, "green,yellow",
    { { EQUIP_QUALITY::LINKED, 3 } });
    map_name_weapon["concussion_missile"] = Weapon(6, "green,yellow",
    { { EQUIP_QUALITY::BREACH, 4 } });
    // Don't have weapon selection logic so just using missiles
    map_name_vehicle["vulture"] = Vehicle("VultureDroid", 6, 2, 0, 3,
    { { map_name_weapon["concussion_missile"] } });
  }
  {
    // Arquitens-Class Light Cruiser (not canon)
    // Assuming capital ships have more skilled gunnery crews
    map_name_weapon["turbolaser_med"] = Weapon(10, "green,green,yellow",
    { { EQUIP_QUALITY::LINKED, 1 }, { EQUIP_QUALITY::BREACH, 3 } });
    map_name_weapon["quad_laser"] = Weapon(6, "green,yellow,yellow",
    { { EQUIP_QUALITY::LINKED, 3 }, { EQUIP_QUALITY::ACCURATE, 1 } });
    map_name_weapon["concussion_missiles"] = Weapon(6, "green,green,yellow",
    { { EQUIP_QUALITY::BREACH, 4 } });

    vector<vector<Weapon>> weapon_clusters;
    for (int i = 0; i < 2; i++) // 4, slow:1 == 2
      weapon_clusters.push_back({ map_name_weapon["turbolaser_med"] });
    for (int i = 0; i < 4; i++) // 4
      weapon_clusters.push_back({ map_name_weapon["quad_laser"] });
    for (int i = 0; i < 2; i++)  // 4, slow:1 == 2
      weapon_clusters.push_back({ map_name_weapon["concussion_missiles"] });
    map_name_vehicle["arquitens"] = Vehicle("Arquitens",
      90, // hull
      6, // armor
      2, // defense
      6,  // silhouette
      weapon_clusters);
  }

  {
    map_name_weapon["tie_cannon"] = Weapon(6, "green,green,green",
    { { EQUIP_QUALITY::LINKED, 1 } });
    map_name_vehicle["tie"] = Vehicle("TIE", 6, 2, 0, 3,
    { { map_name_weapon["tie_cannon"] } });
  }

  {
    map_name_weapon["interceptor_cannon"] = Weapon(6, "green,green,green",
    { { EQUIP_QUALITY::LINKED, 3 } });
    map_name_vehicle["interceptor"] = Vehicle("Interceptor", 6, 2, 0, 3,
    { { map_name_weapon["interceptor_cannon"] } });
  }

  {
    // Starfighter pilots can only use one weapon at a time.
    // TODO: Find way to do weapon selection based on target
    map_name_weapon["xwing_cannon"] = Weapon(6, "green,green,green",
    { { EQUIP_QUALITY::LINKED, 3 } });
    map_name_weapon["proton_torpedo"] = Weapon(8, "green,green,green",
    { { EQUIP_QUALITY::BREACH, 6 } });
    map_name_vehicle["xwing"] = Vehicle("XWing",
      10,  // hull
      3,  // armor
      1,  // defense
      3,  // silhouette
      { { map_name_weapon["xwing_cannon"] } });
    map_name_vehicle["xwing_protons"] = Vehicle("XWing",
      10,  // hull
      3,  // armor
      1,  // defense
      3,  // silhouette
      { { map_name_weapon["proton_torpedo"] } });
  }

  // Assuming capital ships have more skilled gunnery crews
  map_name_weapon["turbolaser_med_twin"] = Weapon(10, "green,green,yellow",
  { { EQUIP_QUALITY::LINKED, 1 } , {EQUIP_QUALITY::BREACH, 3 } });
  map_name_weapon["turbolaser_lgt"] = Weapon(9, "green,green,yellow",
  { {EQUIP_QUALITY::BREACH, 2} });
  {
    map_name_vehicle["corvette"] = Vehicle("Corvette",
      50, // hull
      5, // armor
      2, // defense
      5,  // silhouette
      { { map_name_weapon["turbolaser_med_twin"] },  // 2, slow:1 == 1
        { map_name_weapon["turbolaser_lgt"] },  // 4, slow:1 == 2
        { map_name_weapon["turbolaser_lgt"] } });
  }

  // Assuming capital ships have more skilled gunnery crews
  map_name_weapon["turbolaser_hvy"] = Weapon(11, "green,green,yellow,yellow",
  { { EQUIP_QUALITY::LINKED, 1 },{ EQUIP_QUALITY::BREACH, 4 } });
  map_name_weapon["ion_turret"] = Weapon(9, "green,yellow,yellow",
  { { EQUIP_QUALITY::BREACH, 3 } });
  map_name_weapon["ion_med"] = Weapon(6, "green,green,yellow");

  {
    vector<vector<Weapon>> ISD_weapons;
    for (int i = 0; i < 10; i++)  // 30, slow:2 == 10
      ISD_weapons.push_back({ map_name_weapon["turbolaser_hvy"] });
    for (int i = 0; i < 15; i++)  // 30, slow:1 == 15
      ISD_weapons.push_back({ map_name_weapon["turbolaser_lgt"] });
    for (int i = 0; i < 10; i++)  // 30, slow:2 == 10
      ISD_weapons.push_back({ map_name_weapon["ion_turret"] });
    for (int i = 0; i < 30; i++)  // 30
      ISD_weapons.push_back({ map_name_weapon["ion_med"] });
    map_name_vehicle["isd"] = Vehicle("ISD",
      145, // hull
      10, // armor
      3, // defense
      8,  // silhouette
      ISD_weapons);
  }

  // Assuming capital ships have more skilled gunnery crews
  map_name_weapon["turbolaser_med"] = Weapon(10, "green,green,yellow,yellow",
  { { EQUIP_QUALITY::BREACH, 3 } });
  map_name_weapon["turbolaser_lgt_quad"] = Weapon(9, "green,yellow,yellow",
  { { EQUIP_QUALITY::LINKED, 3 } ,{ EQUIP_QUALITY::BREACH, 2 } });
  map_name_weapon["defense_laser"] = Weapon(5, "green,green,yellow",
  { { EQUIP_QUALITY::ACCURATE, 1 } });

  {
    vector<vector<Weapon>> vind_weapons;
    for (int i = 0; i < 7; i++) // 15, slow:1 ~= 7
      vind_weapons.push_back({ map_name_weapon["turbolaser_med"] });
    for (int i = 0; i < 5; i++) // 10, slow:1 == 5
      vind_weapons.push_back({ map_name_weapon["turbolaser_lgt_quad"] });
    for (int i = 0; i < 30; i++)  // 30
      vind_weapons.push_back({ map_name_weapon["defense_laser"] });
    map_name_vehicle["vindicator"] = Vehicle("Vindicator",
      85, // hull
      6, // armor
      2, // defense
      7,  // silhouette
      vind_weapons);
  }

  // Assuming capital ships have more skilled gunnery crews
  map_name_weapon["turbolaser_med_dread"] = Weapon(10, "green,green,yellow,yellow",
  { { EQUIP_QUALITY::LINKED, 1 } , { EQUIP_QUALITY::BREACH, 3 } });
  map_name_weapon["turbolaser_lgt_dread"] = Weapon(9, "green,yellow,yellow",
  { { EQUIP_QUALITY::LINKED, 3 } , { EQUIP_QUALITY::BREACH, 2 } });
  map_name_weapon["laser_hvy"] = Weapon(6, "green,green,yellow");

  {
    vector<vector<Weapon>> dread_weapons;
    for (int i = 0; i < 3; i++)  // 10, slow:2 ~= 3
      dread_weapons.push_back({ map_name_weapon["turbolaser_med_dread"] });
    for (int i = 0; i < 7; i++)  // 20, slow:2 ~= 7
      dread_weapons.push_back({ map_name_weapon["turbolaser_lgt_dread"] });
    for (int i = 0; i < 10; i++)  // 10
      dread_weapons.push_back({ map_name_weapon["laser_hvy"] });
    map_name_vehicle["dreadnought"] = Vehicle("Dreadnought",
      100, // hull
      7, // armor
      1, // defense
      7,  // silhouette
      dread_weapons);
  }
}

Weapon VehicleDatabase::get_weapon(string weapon_name) {
  return map_name_weapon[weapon_name];
}
Vehicle VehicleDatabase::get_vehicle(string vehicle_name) {
  return map_name_vehicle[vehicle_name];
}
