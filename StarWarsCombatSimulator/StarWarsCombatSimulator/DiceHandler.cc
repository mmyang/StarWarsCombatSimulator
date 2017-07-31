#include "stdafx.h"

#include "DiceHandler.h"

#include <iostream>
#include <vector>

#include <time.h>  // time
#include <stdlib.h>  // srand, rand

using namespace std;


DiceHandler::DiceHandler() {
    srand(time(NULL));

    map_dice_color["debug"] = DICE_NAME::DEBUG;
    map_dice_color["green"] = DICE_NAME::ABILITY;
    map_dice_color["yellow"] = DICE_NAME::PROFICIENCY;
    map_dice_color["purple"] = DICE_NAME::DIFFICULTY;
    map_dice_color["red"] = DICE_NAME::CHALLENGE;
    map_dice_color["blue"] = DICE_NAME::BOOST;
    map_dice_color["black"] = DICE_NAME::SETBACK;

    map_dice[DICE_NAME::DEBUG] = DiceType(DICE_NAME::DEBUG, {
      { DICE_SYMBOL::ADVANTAGE },
      //{ DICE_SYMBOL::SUCCESS },
      //{ DICE_SYMBOL::TRIUMPH },
      //{ DICE_SYMBOL::FAILURE },
      //{ DICE_SYMBOL::DESPAIR },
      { DICE_SYMBOL::THREAT } });
    map_dice[DICE_NAME::BOOST] = DiceType(DICE_NAME::BOOST, {
      {},{},
      { DICE_SYMBOL::ADVANTAGE },{ DICE_SYMBOL::SUCCESS },
      { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::ADVANTAGE },
      { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::SUCCESS } });
    map_dice[DICE_NAME::SETBACK] = DiceType(DICE_NAME::SETBACK, {
      {},{},
      { DICE_SYMBOL::THREAT },{ DICE_SYMBOL::THREAT },
      { DICE_SYMBOL::FAILURE },{ DICE_SYMBOL::FAILURE } });
    map_dice[DICE_NAME::ABILITY] = DiceType(DICE_NAME::ABILITY, { {},
    { DICE_SYMBOL::ADVANTAGE },{ DICE_SYMBOL::ADVANTAGE },
    { DICE_SYMBOL::SUCCESS },{ DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::ADVANTAGE },
    { DICE_SYMBOL::SUCCESS,DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::SUCCESS } });
    map_dice[DICE_NAME::DIFFICULTY] = DiceType(DICE_NAME::DIFFICULTY, { {},
    { DICE_SYMBOL::THREAT },{ DICE_SYMBOL::THREAT },{ DICE_SYMBOL::THREAT },
    { DICE_SYMBOL::FAILURE },
    { DICE_SYMBOL::THREAT,DICE_SYMBOL::THREAT },
    { DICE_SYMBOL::FAILURE,DICE_SYMBOL::FAILURE },
    { DICE_SYMBOL::THREAT,DICE_SYMBOL::FAILURE } });
    map_dice[DICE_NAME::PROFICIENCY] = DiceType(DICE_NAME::PROFICIENCY, { {},
    { DICE_SYMBOL::ADVANTAGE },
    { DICE_SYMBOL::SUCCESS },{ DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::TRIUMPH },
    { DICE_SYMBOL::ADVANTAGE,DICE_SYMBOL::ADVANTAGE },
    { DICE_SYMBOL::ADVANTAGE,DICE_SYMBOL::ADVANTAGE },
    { DICE_SYMBOL::SUCCESS,DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::SUCCESS,DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::SUCCESS },
    { DICE_SYMBOL::ADVANTAGE, DICE_SYMBOL::SUCCESS } });
    map_dice[DICE_NAME::CHALLENGE] = DiceType(DICE_NAME::CHALLENGE, { {},
    { DICE_SYMBOL::THREAT },{ DICE_SYMBOL::THREAT },
    { DICE_SYMBOL::FAILURE },{ DICE_SYMBOL::FAILURE },
    { DICE_SYMBOL::DESPAIR },
    { DICE_SYMBOL::THREAT, DICE_SYMBOL::THREAT },
    { DICE_SYMBOL::THREAT, DICE_SYMBOL::THREAT },
    { DICE_SYMBOL::FAILURE, DICE_SYMBOL::FAILURE },
    { DICE_SYMBOL::FAILURE, DICE_SYMBOL::FAILURE },
    { DICE_SYMBOL::THREAT, DICE_SYMBOL::FAILURE },
    { DICE_SYMBOL::THREAT, DICE_SYMBOL::FAILURE } });

    map_symbol_name[DICE_SYMBOL::SUCCESS] = "SUCCESS";
    map_symbol_name[DICE_SYMBOL::TRIUMPH] = "TRIUMPH";
    map_symbol_name[DICE_SYMBOL::ADVANTAGE] = "ADVANTAGE";
    map_symbol_name[DICE_SYMBOL::FAILURE] = "FAILURE";
    map_symbol_name[DICE_SYMBOL::DESPAIR] = "DESPAIR";
    map_symbol_name[DICE_SYMBOL::THREAT] = "THREAT";
  }


void DiceHandler::dice_cancellation(map<DICE_SYMBOL, int>& results) {
  tuple<int, int> pair;
  pair = cancel_pairs(results[DICE_SYMBOL::SUCCESS], results[DICE_SYMBOL::FAILURE]);
  results[DICE_SYMBOL::SUCCESS] = get<0>(pair);
  results[DICE_SYMBOL::FAILURE] = get<1>(pair);
  pair = cancel_pairs(results[DICE_SYMBOL::TRIUMPH], results[DICE_SYMBOL::DESPAIR]);
  results[DICE_SYMBOL::TRIUMPH] = get<0>(pair);
  results[DICE_SYMBOL::DESPAIR] = get<1>(pair);
  pair = cancel_pairs(results[DICE_SYMBOL::ADVANTAGE], results[DICE_SYMBOL::THREAT]);
  results[DICE_SYMBOL::ADVANTAGE] = get<0>(pair);
  results[DICE_SYMBOL::THREAT] = get<1>(pair);
}


tuple<int, int> DiceHandler::cancel_pairs(int first_count, int second_count) {
  // Return how many of the first and second remain after cancellation.
  if (first_count > second_count)
    return make_tuple(first_count - second_count, 0);
  else
    return make_tuple(0, second_count - first_count);
}


void DiceHandler::combine_results(map<DICE_SYMBOL, int>& results, map<DICE_SYMBOL, int>& addition) {
  for (const auto& it : addition) {
    results[it.first] += it.second;
  }
}


void DiceHandler::print_roll(map<DICE_SYMBOL, int> results) {
  cout << "Results: ";
  for (const auto& it : results) {
    for (int i = 0; i < it.second; i++) {
      cout << map_symbol_name[it.first] << " ";
    }
  }
  cout << "\n";
}


DICE_NAME DiceHandler::resolve_dice_name(string color) {
  // Notify if color doesn't match any dice
  map<string, DICE_NAME>::iterator it = map_dice_color.find(color);
  if (it == map_dice_color.end()) {
    if (color.find(" ") != string::npos)
      cout << "Don't put spaces in string.\n";
    cout << "Error, color isn't a dice: [" << color << "]\n";
    throw exception();
  }
  return it->second;
}


map<DICE_SYMBOL, int> DiceHandler::roll_all(string dice_str) {
  map<DICE_SYMBOL, int> results;

  string delimiter = ",";
  size_t pos = 0;
  string token;
  while ((pos = dice_str.find(delimiter)) != string::npos) {
    token = dice_str.substr(0, pos);
    combine_results(results, roll(resolve_dice_name(token)));
    dice_str.erase(0, pos + delimiter.length());
  }  // up to last delimiter
  combine_results(results, roll(resolve_dice_name(dice_str)));

  dice_cancellation(results);
  return results;
}


map<DICE_SYMBOL, int> DiceHandler::roll(DICE_NAME dice_name) {
  map<DICE_SYMBOL, int> results;
  DiceType die = map_dice[dice_name];

  vector<DICE_SYMBOL> symbols = die.faces[rand() % die.faces.size()];
  for (int i = 0; i < symbols.size(); i++) {
    results[symbols[i]] += 1;
  }
  return results;
}


int DiceHandler::net_successes(map<DICE_SYMBOL, int> results) {
  // Used for combat.  Extra success = extra damage.  Triumph count as success.
  // Call this after combine_results() used to cancel dice.
  // TODO: Does DESPAIR count as failure or just cancel TRIUMPH?
  return results[DICE_SYMBOL::SUCCESS] + results[DICE_SYMBOL::TRIUMPH];
}

string DiceHandler::defense_dice_str(int num_dice) {
  string result;
  for (int i = 0; i < num_dice; i++) {
    result += ",black";  // 1 SETBACK per defense
  }
  return result;
}

