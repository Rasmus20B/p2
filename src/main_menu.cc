module;

#include <raylib.h>
#include "include/raygui.h"

import types;

export module main_menu;

export u32 main_menu() {

  while(!WindowShouldClose()) {
  BeginDrawing();
  if(GuiButton(Rectangle {
        200, 100, 400, 100
        }, "start")) {
    return 1;
  }
  if(GuiButton(Rectangle {
        200, 200, 400, 100
        }, "quit")) {
    return 0;
  }
  EndDrawing();
}
}
