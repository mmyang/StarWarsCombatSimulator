#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;


enum EQUIP_QUALITY {
  LINKED,
  BREACH,
  ION,  // TODO: Need "ION" and strain mechanics
  ACCURATE,  // +BOOST per level on attack
};

class Weapon {
public:
  Weapon() {}
  Weapon(int a_damage, string a_skill_dice) {
    damage = a_damage;
    skill_dice = a_skill_dice;
  }
  Weapon(int a_damage, string a_skill_dice, map<EQUIP_QUALITY, int> a_qualities)
    : Weapon(a_damage, a_skill_dice) {
    qualities = a_qualities;
  }
  Weapon(const Weapon& original) // copy constructor
    : Weapon(original.damage, original.skill_dice, original.qualities) {}
  int damage;
  string skill_dice;
  map<EQUIP_QUALITY, int> qualities;
};

class Vehicle {
public:
  Vehicle() {}
  Vehicle(string a_name, int a_hit_threshold, int a_armor, int a_defense, int a_sil) :
    starting_hull(a_hit_threshold) {
    name = a_name;
    hit_threshold = a_hit_threshold;
    armor = a_armor;
    defense = a_defense;
    sil = a_sil;
  }
  Vehicle(string a_name, int a_hit_threshold, int a_armor, int a_defense, int a_sil,
    vector<vector<Weapon>> a_weapon_clusters) :
    Vehicle(a_name, a_hit_threshold, a_armor, a_defense, a_sil) {
    weapon_clusters = a_weapon_clusters;
  }
  Vehicle(const Vehicle& original) // copy constructor
    : Vehicle(original.name, original.hit_threshold,
      original.armor, original.defense, original.sil){
    starting_hull = original.starting_hull;
    for (vector<Weapon> cluster : original.weapon_clusters)
      weapon_clusters.push_back(cluster);
  }
  void print_stats() {
    cout << name << " [hull: " << hit_threshold << "/" << starting_hull << "]\n";
  }
  string name;
  int starting_hull;
  int hit_threshold;
  int armor;
  int defense;
  int sil;  // silhouette, modifies difficulty when size difference
  // Each weapon_cluster can target one enemy.
  // Capital ships can target multiple enemies using multiple clusters.
  vector<vector<Weapon>> weapon_clusters;
};

class VehicleDatabase {
public:
  VehicleDatabase();

  Weapon get_weapon(string weapon_name);
  Vehicle get_vehicle(string vehicle_name);

  map<string, Weapon> map_name_weapon;
  map<string, Vehicle> map_name_vehicle;
};
