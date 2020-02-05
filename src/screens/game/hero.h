//
// Created by Pablo Weremczuk on 2/3/20.
//

#ifndef RAYLIBTEST_HERO_H
#define RAYLIBTEST_HERO_H

#include "../../utils/spine_utils.h"

void hero_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, rendereable_t, rendereables, 1);

    for (int i = 0; i < rows->count; i++) {
        rendereables[i].position = (Vector3) {0, 0, 0};
        rendereables[i].asset_type = ASSET_TYPE_HERO;
        rendereables[i].asset = &spine_assets[SPINE_ASSETS_HERO];
    }
}

void hero_controls_update(ecs_rows_t *rows) {
    ECS_COLUMN(rows, rendereable_t, rendereables, 1);

    bool moved = false;

    const int speed = 100;

    for (int i = 0; i < 1; i++) {
        sp_asset_t *sp_asset = (sp_asset_t *)rendereables[i].asset;
        spSkeleton *sp_asset_skeleton = sp_asset->skeleton;
        if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            rendereables[i].position.x -= speed * rows->delta_time;
            sp_asset_skeleton->scaleX = sp_asset_skeleton->scaleX > 0 ? -sp_asset_skeleton->scaleX : sp_asset_skeleton->scaleX;
            moved = true;
        }
        if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            rendereables[i].position.x += speed * rows->delta_time;
            sp_asset_skeleton->scaleX =
                    sp_asset_skeleton->scaleX < 0 ? -sp_asset_skeleton->scaleX : sp_asset_skeleton->scaleX;
            moved = true;
        }
        if (IsKeyDown(KEY_UP) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
            rendereables[i].position.z -= speed * rows->delta_time;
            moved = true;
        }
        if (IsKeyDown(KEY_DOWN) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
            rendereables[i].position.z += speed * rows->delta_time;
            moved = true;
        }
        if (!moved) {
            sp_set_animation(sp_asset, "idle");
        } else {
            sp_set_animation(sp_asset, "run");
        }
    }
}

void camera_update(ecs_rows_t *rows){
    game_context_t *game_context = (game_context_t *)ecs_get_system_context(rows->world, rows->system);
    if (IsKeyDown(KEY_A) || IsGamepadButtonDown(GAMEPAD_PLAYER2, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        game_context->camera.position.x -= 100 * rows->delta_time;
        game_context->camera.target.x -= 100 * rows->delta_time;
    }
    if (IsKeyDown(KEY_D) || IsGamepadButtonDown(GAMEPAD_PLAYER2, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        game_context->camera.position.x += 100 * rows->delta_time;
        game_context->camera.target.x += 100 * rows->delta_time;
    }
    if (IsKeyDown(KEY_W) || IsGamepadButtonDown(GAMEPAD_PLAYER2, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        game_context->camera.position.z -= 100 * rows->delta_time;
        game_context->camera.target.z -= 100 * rows->delta_time;
    }
    if (IsKeyDown(KEY_S) || IsGamepadButtonDown(GAMEPAD_PLAYER2, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        game_context->camera.position.z += 100 * rows->delta_time;
        game_context->camera.target.z += 100 * rows->delta_time;
    }
}

//void enemy_init(ecs_rows_t *rows) {
//    ECS_COLUMN(rows, Vector3, positions, 1);
//    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);
//
//    for (int i = 0; i < rows->count; i++) {
//        positions[i] = (Vector3) {GetRandomValue(500, 2000), 0, GetRandomValue(-100, 300)};
//        sp_assets[i] = spine_assets[SPINE_ASSETS_WOLF];
//    }
//}

void enemy_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, rendereable_t, rendereables, 1);

    for (int i = 0; i < rows->count; i++) {
//        rendereables[i].position = (Vector3) {GetRandomValue(500, 2000), 0, GetRandomValue(-100, 300)};
        rendereables[i].position = (Vector3) {100, 0, 0};
        rendereables[i].asset_type = ASSET_TYPE_ENEMY;
        rendereables[i].asset = &spine_assets[SPINE_ASSETS_WOLF];
    }
}

void rendereables_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, rendereable_t, rendereables, 1);
    ECS_COLUMN_COMPONENT(rows, tag_hero, 2);
    ECS_COLUMN_COMPONENT(rows, tag_enemy, 3);
    for (int i = 0; i < rows->count; i++) {
        if( i < 1){
            rendereables[i].position = (Vector3) {0, 0, 0};
            rendereables[i].asset_type = ASSET_TYPE_HERO;
            rendereables[i].asset = &spine_assets[SPINE_ASSETS_HERO];
            ecs_add(rows->world, rows->entities[i], tag_hero);
        } else if (i < 10) {
            rendereables[i].position = (Vector3) {100, 0, 0};
            rendereables[i].asset_type = ASSET_TYPE_ENEMY;
            rendereables[i].asset = &spine_assets[SPINE_ASSETS_WOLF];
            ecs_add(rows->world, rows->entities[i], tag_enemy);
        }
    }
}

#endif //RAYLIBTEST_HERO_H
