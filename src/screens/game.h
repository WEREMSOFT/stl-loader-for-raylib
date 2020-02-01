//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_GAME_H
#define RAYLIBTEST_GAME_H

#include <math.h>

// Components can't have the same type twice, so we create an alias
typedef Vector3 VectorVelocity3;

void cube_init_position(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);

    for (int i = 0; i < rows->count; i++) {
        position[i].x = position[i].z = 0;
        position[i].y = -25;
        velocities[i].x = velocities[i].y = velocities[i].z = 100;
    }
}

void sp_asset_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    sp_asset_t *dragon = ecs_get_system_context(rows->world, rows->system);

    float phase = PI2 / rows->count;

    for (int i = 0; i < rows->count; i++) {
        position[i].x = 400 + sin(i * phase) * 100;
        position[i].y = 200 + cos(i * phase) * 100;
        position[i].z = 0;
        sp_assets[i] = *dragon;
    }
}


void pre_render(ecs_rows_t *rows) {
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    BeginDrawing();

    ClearBackground(RAYWHITE);
    BeginMode3D(game_context->camera);
    DrawGrid(100, 100.0f);
}

void post_render(ecs_rows_t *rows) {
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    DrawFPS(10, 10);
    if (GuiButton((Rectangle) {20, 50, 140, 30}, "<< BACK TO MAIN MENU")) {
        printf("otro click!!\n");
        game_context->world = screens[SCREEN_MAIN_MENU];
    }

    if(IsKeyPressed(KEY_BACKSPACE)){
        game_context->world = screens[SCREEN_MAIN_MENU];
    }

    EndDrawing();
}

static void render_game(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i++) {
        DrawCube(position[i], 50.0f, 50.0f, 10.0f, CLITERAL(Color) {11, 110, 176, 255});
        DrawCubeWires(position[i], 50.0f, 50.0f, 10.0f, MAROON);
    }
}

static void control_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);

    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    for(int i=0; i < rows->count; i++){
        if(IsKeyDown(KEY_LEFT)){
            positions[i].x += velocities[i].x * rows->delta_time;
        }
        if(IsKeyDown(KEY_RIGHT)){
            positions[i].x -= velocities[i].x * rows->delta_time;
        }
        if(IsKeyDown(KEY_UP)){
            positions[i].z -= velocities[i].z * rows->delta_time;
        }
        if(IsKeyDown(KEY_DOWN)){
            positions[i].z += velocities[i].z * rows->delta_time;
        }
    }

        if(IsKeyDown(KEY_A)){
            game_context->camera.position.x += 100 * rows->delta_time;
            game_context->camera.target.x += 100 * rows->delta_time;
        }
        if(IsKeyDown(KEY_D)){
            game_context->camera.position.x -= 100 * rows->delta_time;
            game_context->camera.target.x -= 100 * rows->delta_time;
        }
        if(IsKeyDown(KEY_W)){
            game_context->camera.position.z -= 100 * rows->delta_time;
            game_context->camera.target.z -= 100 * rows->delta_time;
        }
        if(IsKeyDown(KEY_S)){
            game_context->camera.position.z += 100 * rows->delta_time;
            game_context->camera.target.z += 100 * rows->delta_time;
        }
}

void render_sp_assets(ecs_rows_t *rows) {
    EndMode3D();
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, assets, 2);
    spAnimationState_update(assets[0].animationState, GetFrameTime());
    spAnimationState_apply(assets[0].animationState, assets[0].skeleton);
    spSkeleton_updateWorldTransform(assets[0].skeleton);

    for (int i = 0; i < rows->count; i++) {
        drawSkeleton(assets[i].skeleton, position[i]);
    }
}

void init_game_world(ecs_world_t *world, game_context_t *game_context) {
    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, VectorVelocity3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_TAG(world, tag_rendereable);
    ECS_TAG(world, tag_hero);

    ECS_SYSTEM(world, control_hero, EcsOnUpdate, Vector3, VectorVelocity3);
    ECS_SYSTEM(world, pre_render, EcsOnUpdate, tag_rendereable);
    ECS_SYSTEM(world, render_game, EcsOnUpdate, Vector3, tag_rendereable);
    ECS_SYSTEM(world, render_sp_assets, EcsOnUpdate, Vector3, sp_asset_t);
    ECS_SYSTEM(world, post_render, EcsOnUpdate, tag_rendereable);
    ECS_SYSTEM(world, cube_init_position, EcsOnAdd, Vector3, VectorVelocity3, tag_rendereable);
    ECS_SYSTEM(world, sp_asset_init, EcsOnAdd, Vector3, sp_asset_t);

    ecs_set_system_context(world, pre_render, game_context);
    ecs_set_system_context(world, control_hero, game_context);
    ecs_set_system_context(world, post_render, game_context);
    ecs_set_system_context(world, sp_asset_init, &spine_assets[DRAGON]);

    ECS_ENTITY(world, cube, Vector3, VectorVelocity3, tag_rendereable, tag_hero);

    ECS_TYPE(world, Dragon, Vector3, sp_asset_t);
    ecs_new_w_count(world, Dragon, 6);

    ecs_set_target_fps(world, 60);
}

#endif //RAYLIBTEST_GAME_H
