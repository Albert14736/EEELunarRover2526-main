// --- The entire control web page (HTML + CSS + JavaScript) is stored in PROGMEM ---
// PROGMEM puts the page in the 256 KB program FLASH, NOT in the tiny 32 KB of RAM.
// R"rawliteral( ... )rawliteral" lets us paste raw, multi-line HTML straight in
// without having to escape any of the quotes.
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head> ... CSS styles ... </head>
<body>
  <!-- direction buttons, the speed slider, the live sensor dashboard, and the
       JavaScript that sends drive commands and polls the sensors -->
</body>
</html>
)rawliteral";

// Short, fixed strings use the F() macro for the same reason:
// F("...") is also kept in flash instead of being copied into RAM.
void replyAPI(const String& msg) {
  server.send(200, F("text/plain"), msg);   // the "text/plain" literal stays in flash
}
