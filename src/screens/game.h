//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_GAME_H
#define RAYLIBTEST_GAME_H

#include <math.h>
#include "game/types.h"
#include "game/bullets.h"
#include "../utils/stack.h"
#include "game/hero.h"
#include <stdlib.h>
#include <flecs.h>
#include <flecs/util/stats.h>

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

int vectors_3_compare_z(const void *vector_a, const void *vector_b) {
    Vector3 *va, *vb;
    int a, b;

    va = ((Vector3 *) vector_a);
    vb = ((Vector3 *) vector_b);

    a = (int) 100.f * va->z;
    b = (int) 100.f * vb->z;

    return a - b;
}

int rendereables_compare_z(const void *rendereable_a, const void *rendereable_b) {
    rendereable_t *va, *vb;
    int a, b;

    va = ((rendereable_t *) rendereable_a);
    vb = ((rendereable_t *) rendereable_b);

    a = (int) 100.f * va->position.z;
    b = (int) 100.f * vb->position.z;

    return b - a;
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
    ECS_COLUMN(rows, rendereable_t, rendereables, 1);

    for (int i = 0; i < rows->count; i++) {
        rendereables[i].position.x = GetRandomValue(0, 160) * 12;
        rendereables[i].position.z = GetRandomValue(-200, 350);
        rendereables[i].position.y = 0;
        rendereables[i].asset_type = ASSET_TYPE_TREE_1;
        rendereables[i].asset = &texture_assets[TEXTURES_TREE];
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

void sp_assets_animation_update(ecs_rows_t *rows) {
    for (int i = 0; i < SPINE_ASSSETS_COUNT; i++) {
        spAnimationState_update(spine_assets[i].animationState, rows->delta_time);
        spAnimationState_apply(spine_assets[i].animationState, spine_assets[i].skeleton);
        spSkeleton_updateWorldTransform(spine_assets[i].skeleton);
    }
}

void rendereables_push_into_render_stack(ecs_rows_t *rows) {
    ECS_COLUMN(rows, rendereable_t, rendereables, 1);
    ECS_COLUMN(rows, rendereables_stack_t, render_stack, 2);

    for (int i = 0; i < rows->count; i++) {
        stack_push(render_stack, rendereables[i]);
    }
}

void process_render_stack(ecs_rows_t *rows) {
    ECS_COLUMN(rows, rendereables_stack_t, render_stack, 1);
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    qsort(render_stack->rendereables, render_stack->index, sizeof(rendereable_t), rendereables_compare_z);

    rendereable_t rendereable;
    while (stack_pop(render_stack, &rendereable) != -1) {
        switch (rendereable.asset_type) {
            case ASSET_TYPE_HERO:
                drawSkeleton(((sp_asset_t *) rendereable.asset)->skeleton, rendereable.position);
                break;
            case ASSET_TYPE_ENEMY:
                drawSkeleton(((sp_asset_t *) rendereable.asset)->skeleton, rendereable.position);
                break;
            case ASSET_TYPE_BUSH:
                printf("rendering bush\n");
                break;
            case ASSET_TYPE_TREE_1:
                    rendereable.position.y += 90.0;
                    DrawBillboard(game_context->camera, *((Texture2D *)rendereable.asset), rendereable.position, 100.0f,
                                  WHITE);
                break;
        }
    }

    stack_clean(render_stack);
}

void game_world_init(ecs_world_t *world, game_context_t *game_context) {

    /* Import modules for admin */
    ECS_IMPORT(world, FlecsStats, 0);

    // --- Create Components
    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, VectorVelocity3);
    ECS_COMPONENT(world, VectorAcceleration3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_COMPONENT(world, Texture2D);
    ECS_COMPONENT(world, rendereable_t);
    ECS_COMPONENT(world, rendereables_stack_t);

    // --- Create Tags
    ECS_TAG(world, tag_hero);
    ECS_TAG(world, tag_enemy);
    ECS_TAG(world, tag_bullet_flying);
    ECS_TAG(world, tag_bullet_idle);
    ECS_TAG(world, tag_billboard);
    ECS_TAG(world, tag_terrain_tile);

    // --- Create Entities
    ECS_ENTITY(world, render_list, rendereables_stack_t);

    // --- Initializers
    ECS_SYSTEM(world, bullets_init, EcsOnAdd, Vector3, VectorVelocity3, VectorAcceleration3, tag_bullet_idle);
    ECS_SYSTEM(world, hero_init, EcsOnAdd, rendereable_t, render_list.rendereables_stack_t, tag_hero);
    ECS_SYSTEM(world, enemy_init, EcsOnAdd, rendereable_t, tag_enemy);
    ECS_SYSTEM(world, billboards_init, EcsOnAdd, rendereable_t, tag_billboard);
    ECS_SYSTEM(world, ground_tiles_init, EcsOnAdd, Vector3, tag_terrain_tile);

    // --- Create entities
    // -- HERO
    ECS_TYPE(world, g_hero_t, rendereable_t, tag_hero);
    ecs_new(world, g_hero_t);

    ECS_TYPE(world, g_enemy_t, rendereable_t, tag_enemy);
    ecs_new_w_count(world, g_enemy_t, 50);

    ECS_TYPE(world, g_billboard, rendereable_t, tag_billboard);
    ecs_new_w_count(world, g_billboard, 100);

    ECS_TYPE(world, bullet_t, Vector3, VectorVelocity3, VectorAcceleration3, tag_bullet_idle);
    ecs_new_w_count(world, bullet_t, 50);

    ECS_TYPE(world, terrain_tile_t, Vector3, tag_terrain_tile);
    ecs_new_w_count(world, terrain_tile_t, 25);

    ecs_set_target_fps(world, 60);

    // --- Update Systems
    ECS_SYSTEM(world, camera_update, 0);
    ecs_set_system_context(world, camera_update, game_context);

    ECS_SYSTEM(world, sp_assets_animation_update, EcsOnUpdate, 0);

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

    ECS_SYSTEM(world, hero_controls_update, EcsOnUpdate, rendereable_t, tag_hero);

    // Render Pipeline
    ECS_SYSTEM(world, pre_render, EcsOnUpdate, 0);
    ecs_set_system_context(world, pre_render, game_context);

    ECS_SYSTEM(world, ground_tiles_render, EcsOnUpdate, Vector3, tag_terrain_tile);

    ECS_SYSTEM(world, rendereables_push_into_render_stack, EcsOnUpdate, rendereable_t,
               render_list.rendereables_stack_t);

    ECS_SYSTEM(world, bullets_render, EcsOnUpdate, Vector3, tag_bullet_flying);
    ECS_SYSTEM(world, spine_assets_render, EcsOnUpdate, Vector3, sp_asset_t);

    ECS_SYSTEM(world, process_render_stack, EcsOnUpdate, render_list.rendereables_stack_t);
    ecs_set_system_context(world, process_render_stack, game_context);

    ECS_SYSTEM(world, post_render, EcsOnUpdate, 0);
    ecs_set_system_context(world, post_render, game_context);

}

#endif //RAYLIBTEST_GAME_H
