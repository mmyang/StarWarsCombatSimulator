// Copyright 2017 Michael Yang
// StarWarsCombatSimulator is released under the MIT license.
//
// Small program to run quick simulations using the rules of the
// Fantasy Flight Star Wars RPG.  I wrote this to test how starship
// fleet battles would go using the base rules.
//
// For example, how many TIE fighters would it take to destroy 4 X-Wings?
// How many X-Wings would it take to destroy an Imperial Star Destroyer?

#include "stdafx.h"


#include <string>
#include <iostream>
#include <map>

#include <time.h>  // time
#include <stdlib.h>  // srand, rand
#include <algorithm>  // std::min

#include "DiceHandler.h"
#include "VehicleDatabase.h"

using namespace std;


DiceHandler g_dice_handler;

void clean_dead_vehicles(vector<Vehicle>& vehicles) {
  for (int i = 0; i < vehicles.size(); i++) {
    if (vehicles[i].hit_threshold < 1) {
      vehicles.erase(vehicles.begin() + i);
      i -= 1;
    }
  }
}
void print_vehicles(vector<Vehicle>& vehicles) {
  for (int i = 0; i < vehicles.size(); i++)
    vehicles[i].print_stats();
}

void attack(vector<Weapon>& cluster, Vehicle& defender, string difficulty) {
  // TODO: Implement critical hits (card-based, not original d100 table)

  map<DICE_SYMBOL, int> results;
  if (defender.defense > 0)
    difficulty += g_dice_handler.defense_dice_str(defender.defense);

  for (auto& weapon : cluster) {
    string dicepool = weapon.skill_dice + "," + difficulty;
    for (int i = 0; i < weapon.qualities[EQUIP_QUALITY::ACCURATE]; i++)
      dicepool += ",blue";
    results = g_dice_handler.roll_all(dicepool);
    //cout << "  -- mmyang: dicepool=" << dicepool << "\n";
    //g_dice_handler.print_roll(results);  // mmyang, debugging
    int net_successes = g_dice_handler.net_successes(results);
    int effective_armor = max(0,
      defender.armor - weapon.qualities[EQUIP_QUALITY::BREACH]);
    int damage_per_hit = net_successes + weapon.damage - effective_armor;
    int total_hits = 0;
    if (net_successes > 0) {
      total_hits = 1;
      if (weapon.qualities[EQUIP_QUALITY::LINKED] > 0) {
        int link_triggers = results[DICE_SYMBOL::ADVANTAGE] / 2;
        total_hits += min(link_triggers, weapon.qualities[EQUIP_QUALITY::LINKED]);
      }
    }
    defender.hit_threshold -= max(0, total_hits * damage_per_hit);
    if (total_hits > 0) {
      cout << "   mmyang: hits==" << total_hits << ", dmg_per=="
        << damage_per_hit;
      if (weapon.qualities[EQUIP_QUALITY::BREACH] > 0)
        cout << " BR(remaining:" << effective_armor << ")\n";
      else
        cout << "\n";
    }
  }
}

string get_silhouette_difficulty(const Vehicle& attacker, const Vehicle& defender) {
  int dif = defender.sil - attacker.sil;
  switch (dif) {
  case 6:
  case 5:
  case 4:
  case 3:
  case 2:  // attacker smaller by 2
    return "purple";
  case 1:
  case 0:
  case -1: // attacker within 1 silhouette of target
    return "purple,purple";
  case -2:  // attacker larger by 2
    return "purple,purple,purple";
  case -3:
    return "purple,purple,purple,purple";
  case -4:  // attacker larger by 4
  case -5:
  case -6:
    return "purple,purple,purple,purple,purple";
  default:
    cout << "Unexpected silhouette difference: " << dif << "\n";
    throw exception();
  }
}

void battle(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  int rounds = 0;
  // TODO: Use carryover THREAT for SETBACK
  // TODO: Use carryover ADVANTAGE for BOOST
  // TODO: Implement target selection so capitals target capitals
  while (defenders.size() > 0 && attackers.size() > 0) {
    rounds++;
    for (auto attacker : attackers) {
      // Each weapon cluster can target a different enemy (for capital ships)
      for (auto cluster : attacker.weapon_clusters) {
        Vehicle& defender = defenders[rand() % defenders.size()];
        attack(cluster, defender, get_silhouette_difficulty(attacker, defender));
      }
    }
    print_vehicles(defenders);

    for (auto defender : defenders) {
      for (auto cluster : defender.weapon_clusters) {
        Vehicle& attacker = attackers[rand() % attackers.size()];
        attack(cluster, attacker, get_silhouette_difficulty(defender, attacker));
      }
    }
    print_vehicles(attackers);

    // TODO: Haven't implemented initiative so resolve attacks simultaneously.
    // TODO: Because attacks simultaneous, not clearing ships immediately on death.
    clean_dead_vehicles(defenders);
    clean_dead_vehicles(attackers);
    cout << "\n";
  }

  cout << "\n";
  for (auto defender : defenders)
    defender.print_stats();
  for (auto attacker : attackers)
    attacker.print_stats();
  cout << "** rounds: " << rounds << "   survivors: "
    << defenders.size() + attackers.size() << "\n";
}

VehicleDatabase vehicle_db;

// Custom Speeders vs Custom AT-ST
void scenario_01(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  // NOTE: These are the custom vehicles from the beginner adventure
  Weapon speeder_blaster(4, "green,green,green");
  for (int i = 0; i < 3; ++i) {
    attackers.push_back(Vehicle("speeder_" + to_string(i), 2, 0, 0, 2,
    { { speeder_blaster } }));
  }

  Weapon walker_cannon(5, "green,green,green",
  { { EQUIP_QUALITY::LINKED, 1 } });
  for (int i = 0; i < 1; ++i) {
    defenders.push_back(Vehicle("walker_" + to_string(i), 15, 3, 0, 3,
    { { walker_cannon } }));
  }
}

// TIEs vs X-Wings
void scenario_02(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 6; ++i) {
    Vehicle tie = vehicle_db.get_vehicle("tie");
    tie.name = tie.name + "_" + to_string(i);
    attackers.push_back(tie);
  }

  for (int i = 0; i < 4; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing");
    xwing.name = xwing.name + "_" + to_string(i);
    defenders.push_back(xwing);
  }
}

// TIEs vs Corvette
void scenario_03(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 9; ++i) {
    Vehicle tie = vehicle_db.get_vehicle("tie");
    tie.name = tie.name + "_" + to_string(i);
    attackers.push_back(tie);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle corvette = vehicle_db.get_vehicle("corvette");
    corvette.name = corvette.name + "_" + to_string(i);
    defenders.push_back(corvette);
  }
}

// X-Wings vs Corvette
void scenario_04(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 4; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing_protons");
    xwing.name = xwing.name + "_" + to_string(i);
    attackers.push_back(xwing);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle corvette = vehicle_db.get_vehicle("corvette");
    corvette.name = corvette.name + "_" + to_string(i);
    defenders.push_back(corvette);
  }
}

// X-Wings vs ISD
void scenario_05(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 29; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing_protons");
    xwing.name = xwing.name + "_" + to_string(i);
    attackers.push_back(xwing);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle isd = vehicle_db.get_vehicle("isd");
    isd.name = isd.name + "_" + to_string(i);
    defenders.push_back(isd);
  }
}

// X-Wings vs ISD + 24 TIE (2 squad)
void scenario_06(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 78; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing_protons");
    xwing.name = xwing.name + "_" + to_string(i);
    attackers.push_back(xwing);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle isd = vehicle_db.get_vehicle("isd");
    isd.name = isd.name + "_" + to_string(i);
    defenders.push_back(isd);
  }
  for (int i = 0; i < 60; ++i) {
    Vehicle tie = vehicle_db.get_vehicle("tie");
    tie.name = tie.name + "_" + to_string(i);
    defenders.push_back(tie);
  }
}

// X-Wings vs Vindicator
void scenario_07(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 10; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing_protons");
    xwing.name = xwing.name + "_" + to_string(i);
    attackers.push_back(xwing);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle vind = vehicle_db.get_vehicle("vindicator");
    vind.name = vind.name + "_" + to_string(i);
    defenders.push_back(vind);
  }
}

// X-Wings vs Vindicator + 24 TIE (2 squad)
void scenario_08(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 32; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing_protons");
    xwing.name = xwing.name + "_" + to_string(i);
    attackers.push_back(xwing);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle vind = vehicle_db.get_vehicle("vindicator");
    vind.name = vind.name + "_" + to_string(i);
    defenders.push_back(vind);
  }
  for (int i = 0; i < 24; ++i) {
    Vehicle tie = vehicle_db.get_vehicle("tie");
    tie.name = tie.name + "_" + to_string(i);
    defenders.push_back(tie);
  }
}

// Dreadnought vs ISD
void scenario_09(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 1; ++i) {
    Vehicle dread = vehicle_db.get_vehicle("dreadnought");
    dread.name = dread.name + "_" + to_string(i);
    attackers.push_back(dread);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle isd = vehicle_db.get_vehicle("isd");
    isd.name = isd.name + "_" + to_string(i);
    defenders.push_back(isd);
  }
}

// Dread + X-Wings vs Vindicator + TIEs
void scenario_10(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  for (int i = 0; i < 3; ++i) {
    Vehicle dread = vehicle_db.get_vehicle("dreadnought");
    dread.name = dread.name + "_" + to_string(i);
    attackers.push_back(dread);
  }
  for (int i = 0; i < 36; ++i) {
    Vehicle xwing = vehicle_db.get_vehicle("xwing_protons");
    xwing.name = xwing.name + "_" + to_string(i);
    attackers.push_back(xwing);
  }

  for (int i = 0; i < 1; ++i) {
    Vehicle vind = vehicle_db.get_vehicle("isd");
    vind.name = vind.name + "_" + to_string(i);
    defenders.push_back(vind);
  }
  for (int i = 0; i < 60; ++i) {
    Vehicle tie = vehicle_db.get_vehicle("tie");
    tie.name = tie.name + "_" + to_string(i);
    defenders.push_back(tie);
  }
}

// Vulture Droids swarm vs Imperial task force
void scenario_11(vector<Vehicle>& attackers, vector<Vehicle>& defenders) {
  // From the book "Thrawn", one ISD + 2 raider-class corvettes
  // + 1 Arquitens-Class light cruiser went up against 400 vulture droids.
  // Would probably need to implement Minion rules to portray this battle.

  for (int i = 0; i < 200; ++i) {
    Vehicle vehicle = vehicle_db.get_vehicle("vulture");
    vehicle.name = vehicle.name + "_" + to_string(i);
    attackers.push_back(vehicle);
  }

  for (int i = 0; i < 72; ++i) {  // Full ISD TIE wing
    Vehicle vehicle = vehicle_db.get_vehicle("tie");
    vehicle.name = vehicle.name + "_" + to_string(i);
    defenders.push_back(vehicle);
  }
  for (int i = 0; i < 1; ++i) {
    Vehicle vehicle = vehicle_db.get_vehicle("isd");
    vehicle.name = vehicle.name + "_" + to_string(i);
    defenders.push_back(vehicle);
  }
  for (int i = 0; i < 1; ++i) {
    Vehicle vehicle = vehicle_db.get_vehicle("arquitens");
    vehicle.name = vehicle.name + "_" + to_string(i);
    defenders.push_back(vehicle);
  }
  for (int i = 0; i < 2; ++i) {
    Vehicle vehicle = vehicle_db.get_vehicle("corvette");
    vehicle.name = vehicle.name + "_" + to_string(i);
    defenders.push_back(vehicle);
  }
}

int main()
{
  srand(time(NULL));
  vector<Vehicle> attackers;
  vector<Vehicle> defenders;
  //scenario_02(attackers, defenders);
  //battle(attackers, defenders);

  int atk_tie_dfn[] = { 0,0,0 };
  for (int i = 0; i < 1; i++) {
    attackers = vector<Vehicle>();
    defenders = vector<Vehicle>();
    scenario_11(attackers, defenders);
    battle(attackers, defenders);
    atk_tie_dfn[0] += (attackers.size() > 0 ? 1 : 0);
    atk_tie_dfn[1] += (attackers.empty() && defenders.empty() ? 1 : 0);
    atk_tie_dfn[2] += (defenders.size() > 0 ? 1 : 0);
  }
  cout << "attackers/tie/defenders: " << atk_tie_dfn[0]
    << "/" << atk_tie_dfn[1] << "/" << atk_tie_dfn[2] << "\n";

  //string dice_str = "green,green,yellow,purple,purple,purple";  // No spaces
  //g_dice_handler.print_roll(g_dice_handler.roll_all(dice_str));
  return 0;
}
