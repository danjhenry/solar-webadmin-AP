#include <Arduino.h>
#include "read-file.h"

int readFile(const char * fpath, String & _buffer) {
  File f = SPIFFS.open(fpath, "r");
  if (!f) { return 1; }

  int _size = f.available();
  char * const cstr = (char *)malloc(_size + 1);
  if (!cstr) {
    f.close();
    return 1;
  }

  cstr[_size] = '\0';
  for (int i = 0; i < _size; i++) {
    cstr[i] = f.read();
  }

  f.close();
  _buffer = cstr;
  free(cstr);

  return 0;
}

int writeFile(const char * fpath, String _buffer) {
  File f = SPIFFS.open(fpath, "w");
  if (!f) { return 1; }
  f.print(_buffer);
  f.flush();
  f.close();
  return 0;
}
