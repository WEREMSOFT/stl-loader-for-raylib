//
// Created by Pablo Weremczuk on 2/3/20.
//

#ifndef RAYLIBTEST_HERO_H
#define RAYLIBTEST_HERO_H

#include "../../utils/spine_utils.h"

void hero_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 3);

    for (int i = 0; i < rows->count; i++) {
        position[i] = (Vector3) {0, 0, 0};
        velocities[i].x = velocities[i].y = velocities[i].z = 100;
        sp_assets[i] = spine_assets[HERO];
    }
}

void hero_sp_asset_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    for (int i = 0; i < rows->count; i++) {
        sp_assets[i] = spine_assets[HERO];
    }
}

void hero_controls_update(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 3);

    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    bool moved = false;

    for (int i = 0; i < rows->count; i++) {
        if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            positions[i].x -= velocities[i].x * rows->delta_time;
            sp_assets[i].skeleton->scaleX =
                    sp_assets[i].skeleton->scaleX > 0 ? -sp_assets[i].skeleton->scaleX : sp_assets[i].skeleton->scaleX;
            moved = true;
        }
        if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            positions[i].x += velocities[i].x * rows->delta_time;
            sp_assets[i].skeleton->scaleX =
                    sp_assets[i].skeleton->scaleX < 0 ? -sp_assets[i].skeleton->scaleX : sp_assets[i].skeleton->scaleX;
            moved = true;
        }
        if (IsKeyDown(KEY_UP) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
            positions[i].z -= velocities[i].z * rows->delta_time;
            moved = true;
        }
        if (IsKeyDown(KEY_DOWN) || IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
            positions[i].z += velocities[i].z * rows->delta_time;
            moved = true;
        }
        if (!moved) {
            sp_set_animation(&sp_assets[i], "idle");
        } else {
            sp_set_animation(&sp_assets[i], "run");
        }
    }

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

void enemy_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    for (int i = 0; i < rows->count; i++) {
        positions[i] = (Vector3) {GetRandomValue(500, 2000), 0, GetRandomValue(-100, 300)};
        sp_assets[i] = spine_assets[DRAGON];
    }
}

#endif //RAYLIBTEST_HERO_H
