// The ~8KB page can't be pushed to the WiFi in one go: its send buffer is small,
// so one big write overflows it and the connection drops (the page never loads).
// Fix: send the page out of FLASH in small 512-byte chunks, with a short pause
// between them so the WiFi buffer has time to drain.
void handleRoot() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, F("text/html"), "");        // send the header first; the body follows in chunks
  int totalLength = sizeof(webpage) - 1;
  int bytesSent   = 0;
  const int CHUNK_SIZE = 512;
  char chunkBuffer[CHUNK_SIZE + 1];
  while (bytesSent < totalLength) {
    int n = (totalLength - bytesSent > CHUNK_SIZE) ? CHUNK_SIZE : totalLength - bytesSent;
    memcpy_P(chunkBuffer, webpage + bytesSent, n);   // copy one chunk OUT of flash (PROGMEM) into RAM
    chunkBuffer[n] = '\0';
    server.sendContent(chunkBuffer);                 // send that 512-byte chunk
    bytesSent += n;
    delay(20);                                       // let the WiFi buffer catch up
  }
  server.sendContent("");                            // empty chunk = end of the response
}
