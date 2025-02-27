#include "./src/defines.hpp"
#undef _IS_ARDUINO_

// MARK: - Libraries

#include <stdio.h>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <string.h>
#include "./src/platform/platform-wasm.hpp"

#include "./src/main.hpp"

// MARK: - Emscripten Entry Point

int main()
{
    input.api = &canvasAPI;
    osuapi.search("");
#ifdef _PRINTF_INFO_
    printf("============================\n");
    printf("OSUino started\n");
#endif
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackPress);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, keyCallbackRelease);
    emscripten_set_main_loop(update, 0, 1);

    return 0;
}