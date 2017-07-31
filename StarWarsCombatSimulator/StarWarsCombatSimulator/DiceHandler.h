#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;


enum DICE_NAME {
  DEBUG,  // debug
  ABILITY,  // green
  PROFICIENCY,  // yellow
  DIFFICULTY,  // purple
  CHALLENGE,  // red
  BOOST,  // blue
  SETBACK,  // black
};
enum DICE_SYMBOL {
  SUCCESS,
  TRIUMPH,
  ADVANTAGE,
  FAILURE,
  DESPAIR,
  THREAT
};

class DiceType {
public:
  DiceType() {}
  DiceType(DICE_NAME a_name, vector<vector<DICE_SYMBOL>> a_faces) {
    name = a_name;
    faces = a_faces;
  }
  DICE_NAME name;
  vector<vector<DICE_SYMBOL>> faces;
};

class DiceHandler {
public:
  DiceHandler();
  void dice_cancellation(map<DICE_SYMBOL, int>& results);
  tuple<int, int> cancel_pairs(int first_count, int second_count);
  void combine_results(map<DICE_SYMBOL, int>& results, map<DICE_SYMBOL, int>& addition);
  void print_roll(map<DICE_SYMBOL, int> results);
  DICE_NAME resolve_dice_name(string color);
  map<DICE_SYMBOL, int> roll_all(string dice_str);
  map<DICE_SYMBOL, int> roll(DICE_NAME dice_name);

  // Used for combat.  Extra success = extra damage.  Triumph count as success.
  int net_successes(map<DICE_SYMBOL, int> results);
  // String representation of added difficulty for attacks
  string defense_dice_str(int num_dice);

private:
  map<string, DICE_NAME> map_dice_color;  // string color to enum
  map<DICE_NAME, DiceType> map_dice;  // enum to class
  map<DICE_SYMBOL, string> map_symbol_name;  // enum to string (for printing)
};
