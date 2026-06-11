// Radio sends the age as text over a UART (Serial1), e.g. "#444#444#...". The line is
// noisy, so we never trust one reading: we keep every clean 3-digit reading from the
// last 2 seconds and show the MODE (most frequent). Noise (44 / 445 / 4443) loses the vote.
// ageVals[] + ageTimes[] = a 64-slot ring buffer of recent readings and their arrival times.

// In loop(): read the UART; any non-digit ends a reading; store only 3-digit ones.
while (Serial1.available() && ageBudget-- > 0) {
  char c = Serial1.read();
  if (c >= '0' && c <= '9') { if (ageLen < AGE_BUF - 1) ageBuf[ageLen++] = c; }
  else {                                              // non-digit ('#', noise) = end of a reading
    if (ageLen == 3) {                                // keep only 3-digit readings
      ageVals[ageHead]  = (ageBuf[0]-'0')*100 + (ageBuf[1]-'0')*10 + (ageBuf[2]-'0');
      ageTimes[ageHead] = millis();
      ageHead = (ageHead + 1) % AGE_N;
    }
    ageLen = 0;
  }
}

// The displayed value = the most frequent reading in the last 2 seconds (the mode):
String ageBest() {
  int bestVal = -1, bestCount = 0;
  for (int i = 0; i < AGE_N; i++) {
    if (ageTimes[i] == 0 || millis() - ageTimes[i] > AGE_WIN) continue;          // skip empty/old
    int c = 0;
    for (int j = 0; j < AGE_N; j++)
      if (millis() - ageTimes[j] <= AGE_WIN && ageVals[j] == ageVals[i]) c++;    // count matches
    if (c > bestCount) { bestCount = c; bestVal = ageVals[i]; }
  }
  return bestVal < 0 ? "no signal" : withDecimal(bestVal);   // 444 -> "4.44"
}
