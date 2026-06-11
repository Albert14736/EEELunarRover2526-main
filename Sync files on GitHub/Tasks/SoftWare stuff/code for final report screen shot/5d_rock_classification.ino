// Each rock type has a signature: which IR tier, ultrasound on/off, magnet up/down.
// Any 2 of the 3 signals already pin down the rock, so we SCORE each rock by how many
// of its 3 conditions match, and accept the one scoring >= 2. This means one bad/missing
// sensor still classifies on the other two; a tie (two rocks score 2) stays "Unknown".
struct RockSig { const char* name; bool ir547; bool usYes; bool magUp; };

String rockType() {
  bool ir547 = IRpulseRate > 400;
  bool ir312 = IRpulseRate >= 240 && IRpulseRate <= 400;
  bool usYes = digitalRead(US_PIN) == HIGH;
  int  mv    = analogRead(MAG_PIN);
  bool magUp = magIsNorth(mv), magDn = magIsSouth(mv);

  static const RockSig sigs[4] = {        //  IR  / ultrasound / magnet
    {"Basaltoid", true,  true,  false},   // 547 /  yes  / down
    {"Gravion",   false, false, false},   // 312 /  no   / down
    {"Regolix",   false, true,  true },   // 312 /  yes  / up
    {"Lunarite",  true,  false, true },   // 547 /  no   / up
  };
  int bestScore = 0, bestIdx = -1; bool tie = false;
  for (int i = 0; i < 4; i++) {
    int s = 0;
    if (sigs[i].ir547 ? ir547 : ir312)  s++;     // IR tier matches?
    if (sigs[i].usYes ? usYes : !usYes) s++;     // ultrasound matches?
    if (sigs[i].magUp ? magUp : magDn)  s++;     // magnet matches?
    if (s > bestScore)       { bestScore = s; bestIdx = i; tie = false; }
    else if (s == bestScore) { tie = true; }     // another rock ties the best score
  }
  if (bestIdx >= 0 && bestScore >= 2 && !tie) return sigs[bestIdx].name;
  return "Unknown";                              // <2 matches, or a tie -> don't guess
}
