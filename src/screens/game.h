//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_GAME_H
#define RAYLIBTEST_GAME_H

#include <math.h>
#include "game/types.h"
#include "game/bullets.h"
#include "game/hero.h"

/**
 * Sets stuff like background color and camera. Starts the 3D mode.
 * @param rows
 */
void pre_render(ecs_rows_t *rows) {
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    UpdateCamera(&game_context->camera);

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawTexture(texture_assets[TEXTURES_SKY], 0, 0, WHITE);
    BeginMode3D(game_context->camera);
}

void spine_assets_render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, assets, 2);

    for (int i = 0; i < SPINE_ASSSETS_COUNT; i++) {
        spAnimationState_update(spine_assets[i].animationState, rows->delta_time / 2);
        spAnimationState_apply(spine_assets[i].animationState, spine_assets[i].skeleton);
        spSkeleton_updateWorldTransform(spine_assets[i].skeleton);
    }

    for (int i = 0; i < rows->count; i++) {
        drawSkeleton(assets[i].skeleton, position[i]);
    }
}

int vectors_3_compare(const void *vector_a, const void *vector_b) {
    Vector3 *va, *vb;
    int a, b;

    va = ((Vector3 *) vector_a);
    vb = ((Vector3 *) vector_b);

    a = (int) 100.f * va->z;
    b = (int) 100.f * vb->z;

    return a - b;
}

void billboards_back_render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    Vector3 positions_copy[rows->count];

    for (int i = 0; i < rows->count; i++) {
        positions_copy[i] = positions[i];
    }

    qsort(positions_copy, rows->count, sizeof(Vector3), vectors_3_compare);

    for (int i = 0; i < rows->count / 2; i++) {
        if (i % 2) {
            positions_copy[i].y += 90.0;
            DrawBillboard(game_context->camera, texture_assets[TEXTURES_TREE], positions_copy[i], 100.0f, WHITE);
        } else {
            positions_copy[i].y += 30.0;
            DrawBillboard(game_context->camera, texture_assets[TEXTURES_BUSH], positions_copy[i], 100.0f, WHITE);
        }
    }
}

void billboards_front_render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    Vector3 positions_copy[rows->count];

    for (int i = 0; i < rows->count; i++) {
        positions_copy[i] = positions[i];
    }

    qsort(positions_copy, rows->count, sizeof(Vector3), vectors_3_compare);

    for (int i = rows->count / 2; i < rows->count; i++) {
        if (i % 2) {
            positions_copy[i].y += 90.0;
            DrawBillboard(game_context->camera, texture_assets[TEXTURES_TREE], positions_copy[i], 100.0f, WHITE);
        } else {
            positions_copy[i].y += 30.0;
            DrawBillboard(game_context->camera, texture_assets[TEXTURES_BUSH], positions_copy[i], 100.0f, WHITE);
        }
    }
}

/**
 * Draws the UI, finish the drawing process
 * @param rows
 */
void post_render(ecs_rows_t *rows) {
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    EndMode3D();
    DrawFPS(10, 10);
    if (GuiButton((Rectangle) {20, 50, 140, 30}, "<< BACK TO MAIN MENU")) {
        game_context->world = screens[SCREEN_MAIN_MENU];
    }

    GuiDummyRec((Rectangle) {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 10, 10}, "");

    if (IsKeyPressed(KEY_BACKSPACE)) {
        game_context->world = screens[SCREEN_MAIN_MENU];
    }

    EndDrawing();
}

void billboards_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);

    for (int i = 0; i < rows->count / 2; i++) {
        positions[i].x = GetRandomValue(0, 160) * 12;
        positions[i].z = GetRandomValue(-40, 40) + GetRandomValue(-3, 3) - 200;
        positions[i].y = 0;
    }

    for (int i = rows->count / 2; i < rows->count; i++) {
        positions[i].x = GetRandomValue(0, 160) * 12;
        positions[i].z = GetRandomValue(-40, 40) + GetRandomValue(-3, 3) + 350;
        positions[i].y = 0;
    }
}

void ground_tiles_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);

    int tile_side = (int) sqrtf(rows->count);
    int tile_size = 500;

    int position_index = 0;

    for (int i = 0; i < tile_side; i++) {
        for (int j = 0; j < tile_side; j++) {
            positions[position_index] = (Vector3) {j * tile_size - 500, -5, i * tile_size - 50};
            position_index++;
        }
    }
}

void ground_tiles_render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);

    int tile_size = 500;

    for (int i = 0; i < rows->count; i++) {
        DrawModel(model_assets[MODEL_ASSETS_GROUND_TILE], positions[i], 1.0f, WHITE);
    }
}

void game_world_init(ecs_world_t *world, game_context_t *game_context) {
    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, VectorVelocity3);
    ECS_COMPONENT(world, VectorAcceleration3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_COMPONENT(world, Texture2D);
    ECS_TAG(world, tag_hero);
    ECS_TAG(world, tag_enemy);
    ECS_TAG(world, tag_bullet_flying);
    ECS_TAG(world, tag_bullet_idle);
    ECS_TAG(world, tag_billboard);
    ECS_TAG(world, tag_terrain_tile);

    ECS_SYSTEM(world, bullets_init, EcsOnAdd, Vector3, VectorVelocity3, VectorAcceleration3, tag_bullet_idle);
    ECS_SYSTEM(world, hero_init, EcsOnAdd, Vector3, VectorVelocity3, sp_asset_t, tag_hero);
    ECS_SYSTEM(world, billboards_init, EcsOnAdd, Vector3, tag_billboard);
    ECS_SYSTEM(world, enemy_init, EcsOnAdd, Vector3, sp_asset_t, tag_enemy);
    ECS_SYSTEM(world, ground_tiles_init, EcsOnAdd, Vector3, tag_terrain_tile);

    ECS_SYSTEM(world,
            bullets_flying_update,
            EcsOnUpdate,
            Vector3,
            VectorVelocity3,
            VectorAcceleration3,
            tag_bullet_flying,
            .tag_bullet_idle);

    ecs_set_system_context(world, bullets_flying_update, game_context);

    ECS_SYSTEM(world,
            bullets_idle_update,
            EcsOnUpdate,
            Vector3,
            VectorVelocity3,
            VectorAcceleration3,
            tag_bullet_idle,
            .tag_bullet_flying);

    ecs_set_system_context(world, bullets_idle_update, game_context);


    ECS_SYSTEM(world, hero_controls_update, EcsOnUpdate, Vector3, VectorVelocity3, sp_asset_t, tag_hero);
    ecs_set_system_context(world, hero_controls_update, game_context);

    ECS_SYSTEM(world, pre_render, EcsOnUpdate, 0);
    ecs_set_system_context(world, pre_render, game_context);
    ECS_SYSTEM(world, ground_tiles_render, EcsOnUpdate, Vector3, tag_terrain_tile);

    ECS_SYSTEM(world, billboards_back_render, EcsOnUpdate, Vector3, tag_billboard);
    ecs_set_system_context(world, billboards_back_render, game_context);

    ECS_SYSTEM(world, bullets_render, EcsOnUpdate, Vector3, tag_bullet_flying);
    ECS_SYSTEM(world, spine_assets_render, EcsOnUpdate, Vector3, sp_asset_t);

    ECS_SYSTEM(world, billboards_front_render, EcsOnUpdate, Vector3, tag_billboard);
    ecs_set_system_context(world, billboards_front_render, game_context);

    ECS_SYSTEM(world, post_render, EcsOnUpdate, 0);
    ecs_set_system_context(world, post_render, game_context);

    ECS_ENTITY(world, hero_t, Vector3, VectorVelocity3, sp_asset_t, Texture2D, tag_hero);
    ECS_TYPE(world, enemy_t, Vector3, VectorVelocity3, sp_asset_t, tag_enemy);
    ecs_new_w_count(world, enemy_t, 10);

    ECS_TYPE(world, bullet_t, Vector3, VectorVelocity3, VectorAcceleration3, tag_bullet_idle);
    ecs_new_w_count(world, bullet_t, 50);

    ECS_TYPE(world, billboard_t, Vector3, tag_billboard);
    ecs_new_w_count(world, billboard_t, 100);

    ECS_TYPE(world, terrain_tile_t, Vector3, tag_terrain_tile);
    ecs_new_w_count(world, terrain_tile_t, 25);

    ecs_set_target_fps(world, 60);
}

#endif //RAYLIBTEST_GAME_H
