//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_MAIN_MENU_H
#define RAYLIBTEST_MAIN_MENU_H

#include <flecs.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS

static void render_main_menu(ecs_rows_t *rows) {
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(texture_assets[TEXTURES_OPENING_SCREEN], 0, 0, WHITE);
    DrawFPS(10, 10);
    if (GuiButton((Rectangle) {
                          1000,
                          game_context->screen_size.y / 2,
                          140,
                          100
                  },
                  "START")) {
        game_context->world = screens[SCREEN_GAME];
    }

    if(IsKeyPressed(KEY_ENTER)){
        game_context->world = screens[SCREEN_GAME];
    }
//    if (GuiButton((Rectangle) {20, 50, 140, 30}, GuiIconText(RICON_FILE_OPEN, "Open Image"))) printf("boton!!\n");
//
//    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
//
//    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
//
//    GuiGroupBox((Rectangle) {25, 110, 125, 150}, "STATES");
//    GuiSetState(GUI_STATE_NORMAL);
//    if (GuiButton((Rectangle) {30, 120, 115, 30}, "NORMAL")) {}
//    GuiSetState(GUI_STATE_FOCUSED);
//    if (GuiButton((Rectangle) {30, 155, 115, 30}, "FOCUSED")) {}
//    GuiSetState(GUI_STATE_PRESSED);
//    if (GuiButton((Rectangle) {30, 190, 115, 30}, "#15#PRESSED")) {}
//    GuiSetState(GUI_STATE_DISABLED);
//    if (GuiButton((Rectangle) {30, 225, 115, 30}, "DISABLED")) {}
//    GuiSetState(GUI_STATE_NORMAL);
//
//    GuiComboBox((Rectangle) {25, 470, 125, 30}, "ONE;TWO;THREE;FOUR", NULL);

    EndDrawing();
}

void init_main_menu_world(ecs_world_t *world, game_context_t *game_context) {
    ECS_COMPONENT(world, Vector3);

    ECS_SYSTEM(world, render_main_menu, EcsOnUpdate, 0);

    ecs_set_system_context(world, render_main_menu, game_context);

    ecs_set_target_fps(world, 60);
}

#endif //RAYLIBTEST_MAIN_MENU_H
