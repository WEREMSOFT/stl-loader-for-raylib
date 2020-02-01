//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_GAME_H
#define RAYLIBTEST_GAME_H

#include <math.h>

// Components can't have the same type twice, so we create an alias
typedef Vector3 VectorVelocity3;
typedef Vector3 VectorAcceleration3;
typedef unsigned int state;

enum bullet_state {
    BULLET_STATE_IDLE,
    BULLET_STATE_TRAVELING
};

void init_bullets(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, VectorAcceleration3, accelerations, 3);
    ECS_COLUMN(rows, state, states, 4);


    for (int i = 0; i < rows->count; i++) {
        position[i].z = 0;
        position[i].x = position[i].y = position[i].z = 0;
        velocities[i].x = velocities[i].y = velocities[i].z = 0;
        accelerations[i].x = accelerations[i].z = 0;
        accelerations[i].y = 200;
        states[i] = BULLET_STATE_IDLE;
    }
}

void init_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);

    for (int i = 0; i < rows->count; i++) {
        position[i].x = position[i].y = position[i].z = 0;
        velocities[i].x = velocities[i].y = velocities[i].z = 100;
    }
}

void init_sp_asset(ecs_rows_t *rows) {
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    sp_asset_t *dragon = ecs_get_system_context(rows->world, rows->system);

    for (int i = 0; i < rows->count; i++) {
        sp_assets[i] = *dragon;
    }
}

/**
 * Sets stuff like background color and camera. Starts the 3D mode.
 * @param rows
 */
void pre_render(ecs_rows_t *rows) {
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    BeginDrawing();

    ClearBackground(RAYWHITE);
    BeginMode3D(game_context->camera);
    DrawGrid(100, 100.0f);
}

static void render_bullets(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i++) {
        DrawCube(position[i], 10.0f, 10.0f, 10.0f, CLITERAL(Color) {11, 110, 176, 255});
        DrawCubeWires(position[i], 10.0f, 10.0f, 10.0f, MAROON);
        Vector3 shadow_position = {position[i].x, 0, position[i].z};
        DrawCube(shadow_position,  10, 1, 10, CLITERAL(Color){0, 0, 0, 100});
    }
}

void render_sp_assets(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, sp_asset_t, assets, 2);
    spAnimationState_update(assets[0].animationState, GetFrameTime());
    spAnimationState_apply(assets[0].animationState, assets[0].skeleton);
    spSkeleton_updateWorldTransform(assets[0].skeleton);

    for (int i = 0; i < rows->count; i++) {
        drawSkeleton(assets[i].skeleton, position[i]);
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

    GuiDummyRec((Rectangle){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 10, 10}, "");

    if (IsKeyPressed(KEY_BACKSPACE)) {
        game_context->world = screens[SCREEN_MAIN_MENU];
    }

    EndDrawing();
}

static void control_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);

    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    for (int i = 0; i < rows->count; i++) {
        if (IsKeyDown(KEY_LEFT)) {
            positions[i].x += velocities[i].x * rows->delta_time;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            positions[i].x -= velocities[i].x * rows->delta_time;
        }
        if (IsKeyDown(KEY_UP)) {
            positions[i].z -= velocities[i].z * rows->delta_time;
        }
        if (IsKeyDown(KEY_DOWN)) {
            positions[i].z += velocities[i].z * rows->delta_time;
        }
    }

    if (IsKeyDown(KEY_A)) {
        game_context->camera.position.x += 100 * rows->delta_time;
        game_context->camera.target.x += 100 * rows->delta_time;
    }
    if (IsKeyDown(KEY_D)) {
        game_context->camera.position.x -= 100 * rows->delta_time;
        game_context->camera.target.x -= 100 * rows->delta_time;
    }
    if (IsKeyDown(KEY_W)) {
        game_context->camera.position.z -= 100 * rows->delta_time;
        game_context->camera.target.z -= 100 * rows->delta_time;
    }
    if (IsKeyDown(KEY_S)) {
        game_context->camera.position.z += 100 * rows->delta_time;
        game_context->camera.target.z += 100 * rows->delta_time;
    }
}

void update_bullets(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, VectorAcceleration3, accelerations, 3);
    ECS_COLUMN(rows, state, states, 4);

    bool already_shot = false;

    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    for (int i = 0; i < rows->count; i++) {
        switch (states[i]) {
            case BULLET_STATE_TRAVELING: {
                Vector3 temp_acceleration = Vector3Multiply(accelerations[i], rows->delta_time);

                velocities[i] = Vector3Add(velocities[i], temp_acceleration);

                Vector3 temp_velocity = Vector3Multiply(velocities[i], rows->delta_time);

                positions[i] = Vector3Add(positions[i], temp_velocity);

                float temp_position_y = positions[i].y;

                positions[i].y = fmin(0, positions[i].y);

                if (temp_position_y != positions[i].y) {
                    velocities[i] = Vector3Multiply(velocities[i], 0);
                    positions[i] = Vector3Multiply(positions[i], 0);
                    states[i] = BULLET_STATE_IDLE;
                }
            }
                break;
            case BULLET_STATE_IDLE:
                if(IsKeyPressed(KEY_SPACE) && !already_shot){
                    already_shot = true;
                    states[i] = BULLET_STATE_TRAVELING;
                    float distance_to_target = Vector3Length(game_context->camera.target);
                    velocities[i] = Vector3Multiply(Vector3Normalize(game_context->camera.target), 50);
                    velocities[i].y = -distance_to_target * 2;
                }
                break;
        }
    }
}

void init_game_world(ecs_world_t *world, game_context_t *game_context) {
    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, VectorVelocity3);
    ECS_COMPONENT(world, VectorAcceleration3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_COMPONENT(world, state);
    ECS_TAG(world, tag_ui);
    ECS_TAG(world, tag_hero);
    ECS_TAG(world, tag_bullet);

    ECS_SYSTEM(world, init_bullets, EcsOnAdd, Vector3, VectorVelocity3, VectorAcceleration3, state, tag_bullet);
    ECS_SYSTEM(world, init_hero, EcsOnAdd, Vector3, VectorVelocity3, tag_hero);
    ECS_SYSTEM(world, init_sp_asset, EcsOnAdd, Vector3, sp_asset_t);
    ecs_set_system_context(world, init_sp_asset, &spine_assets[HERO]);

    ECS_SYSTEM(world, update_bullets, EcsOnUpdate, Vector3, VectorVelocity3, VectorAcceleration3, state, tag_bullet);
    ecs_set_system_context(world, update_bullets, game_context);

    ECS_SYSTEM(world, control_hero, EcsOnUpdate, Vector3, VectorVelocity3, tag_hero);
    ecs_set_system_context(world, control_hero, game_context);

    ECS_SYSTEM(world, pre_render, EcsOnUpdate, tag_hero);
    ecs_set_system_context(world, pre_render, game_context);

    ECS_SYSTEM(world, render_bullets, EcsOnUpdate, Vector3, tag_bullet);
    ECS_SYSTEM(world, render_sp_assets, EcsOnUpdate, Vector3, sp_asset_t);

    ECS_SYSTEM(world, post_render, EcsOnUpdate, tag_hero);
    ecs_set_system_context(world, post_render, game_context);

    ECS_ENTITY(world, hero_t, Vector3, VectorVelocity3, sp_asset_t, tag_hero);

    ECS_TYPE(world, bullet_t, Vector3, VectorVelocity3, VectorAcceleration3, state, tag_bullet);
    ecs_new_w_count(world, bullet_t, 50);

    ecs_new(world, tag_ui);

    ecs_set_target_fps(world, 60);
}

#endif //RAYLIBTEST_GAME_H
