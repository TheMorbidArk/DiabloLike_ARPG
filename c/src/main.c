#include "tic80.h"
#include "systems/assets.h"
#include "systems/map.h"
#include "systems/camera.h"
#include "systems/renderer.h"
#include "entities/player.h"

WASM_EXPORT("BOOT")
void BOOT() {
    assets_init();
    map_init();
}

WASM_EXPORT("TIC")
void TIC() {
    player_update();
    camera_update(&player);
    render_scene(&player);
}