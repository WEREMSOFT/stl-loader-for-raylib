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

void sp_set_animation(sp_asset_t *sp_asset, char *animation_name) {
    int track = 0;
    int loop = 1;

    spTrackEntry *te = spAnimationState_getCurrent(sp_asset->animationState, track);

    if (strncmp(animation_name, te->animation->name, 30) != 0) {
        spAnimationState_setAnimationByName(sp_asset->animationState, track, animation_name, loop);
    }
}

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
        accelerations[i].y = -200;
        states[i] = BULLET_STATE_IDLE;
    }
}

void init_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 3);

    for (int i = 0; i < rows->count; i++) {
        position[i] = (Vector3) {0, 0, 0};
//        position[i].y = 35.0f;
        velocities[i].x = velocities[i].y = velocities[i].z = 100;
        sp_assets[i] = spine_assets[HERO];
    }
}

void init_enemy(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    for (int i = 0; i < rows->count; i++) {
        positions[i] = (Vector3) {GetRandomValue(500, 2000), 0, GetRandomValue(-100, 300)};
        sp_assets[i] = spine_assets[DRAGON];
    }
}

void init_sp_asset_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 2);

    for (int i = 0; i < rows->count; i++) {
        sp_assets[i] = spine_assets[HERO];
    }
}


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

static void render_bullets(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i++) {
        DrawCube(position[i], 10.0f, 10.0f, 10.0f, CLITERAL(Color) {11, 110, 176, 255});
        DrawCubeWires(position[i], 10.0f, 10.0f, 10.0f, MAROON);
        Vector3 shadow_position = {position[i].x, 0, position[i].z};
        DrawCube(shadow_position, 10, 1, 10, CLITERAL(Color) {0, 0, 0, 100});
    }
}

void render_sp_assets(ecs_rows_t *rows) {
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

int compare_vectors_3(const void *vector_a, const void *vector_b) {
    Vector3 *va, *vb;
    int a, b;

    va = ((Vector3 *) vector_a);
    vb = ((Vector3 *) vector_b);

    a = (int) 100.f * va->z;
    b = (int) 100.f * vb->z;

    return a - b;
}

void render_billboards_back(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    Vector3 positions_copy[rows->count];

    for (int i = 0; i < rows->count; i++) {
        positions_copy[i] = positions[i];
    }

    qsort(positions_copy, rows->count, sizeof(Vector3), compare_vectors_3);

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

void render_billboards_front(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    Vector3 positions_copy[rows->count];

    for (int i = 0; i < rows->count; i++) {
        positions_copy[i] = positions[i];
    }

    qsort(positions_copy, rows->count, sizeof(Vector3), compare_vectors_3);

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

static void control_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, sp_asset_t, sp_assets, 3);

    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    bool moved = false;


//    if(IsGamepadAvailable(GAMEPAD_PLAYER1)){
//        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) DrawRectangle(225, 132 + 54, 24, 30, RED);
//        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) DrawRectangle(195, 161, 30, 25, RED);
//        if (IsGamepadButtonDown(GAMEPAD_PLAYER1, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) DrawRectangle(195 + 54, 161, 30, 25, RED);
//    }


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

                positions[i].y = fmax(0, positions[i].y);

                if (temp_position_y != positions[i].y) {
                    velocities[i] = Vector3Multiply(velocities[i], 0);
                    positions[i] = Vector3Multiply(positions[i], 0);
                    states[i] = BULLET_STATE_IDLE;
                }
            }
                break;
            case BULLET_STATE_IDLE:
                if ((IsKeyPressed(KEY_SPACE)  || IsGamepadButtonReleased(GAMEPAD_PLAYER2, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) && !already_shot) {
                    already_shot = true;
                    states[i] = BULLET_STATE_TRAVELING;
                    float distance_to_target = Vector3Length(game_context->camera.target);
                    velocities[i] = Vector3Multiply(Vector3Normalize(game_context->camera.target), 50);
                    velocities[i].y = distance_to_target * 2;
                }
                break;
        }
    }
}

void init_billboards(ecs_rows_t *rows) {
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

void render_hero(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);


    for (int i = 0; i < rows->count; i++) {
        DrawBillboard(game_context->camera, texture_assets[TEXTURES_HERO], positions[i], 50.0f, WHITE);
    }
}

void init_ground_tiles(ecs_rows_t *rows) {
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

void render_ground_tiles(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);

    int tile_size = 500;

    for (int i = 0; i < rows->count; i++) {
        DrawModel(model_assets[MODEL_ASSETS_GROUND_TILE], positions[i], 1.0f, WHITE);
    }
}

void init_game_world(ecs_world_t *world, game_context_t *game_context) {
    ECS_COMPONENT(world, Vector3);
    ECS_COMPONENT(world, VectorVelocity3);
    ECS_COMPONENT(world, VectorAcceleration3);
    ECS_COMPONENT(world, sp_asset_t);
    ECS_COMPONENT(world, Texture2D);
    ECS_COMPONENT(world, state);
    ECS_TAG(world, tag_ui);
    ECS_TAG(world, tag_hero);
    ECS_TAG(world, tag_enemy);
    ECS_TAG(world, tag_bullet);
    ECS_TAG(world, tag_billboard);
    ECS_TAG(world, tag_terrain_tile);

    ECS_SYSTEM(world, init_bullets, EcsOnAdd, Vector3, VectorVelocity3, VectorAcceleration3, state, tag_bullet);
    ECS_SYSTEM(world, init_hero, EcsOnAdd, Vector3, VectorVelocity3, sp_asset_t, tag_hero);
    ECS_SYSTEM(world, init_billboards, EcsOnAdd, Vector3, tag_billboard);
    ECS_SYSTEM(world, init_enemy, EcsOnAdd, Vector3, sp_asset_t, tag_enemy);
    ECS_SYSTEM(world, init_ground_tiles, EcsOnAdd, Vector3, tag_terrain_tile);

    ECS_SYSTEM(world, update_bullets, EcsOnUpdate, Vector3, VectorVelocity3, VectorAcceleration3, state, tag_bullet);
    ecs_set_system_context(world, update_bullets, game_context);

    ECS_SYSTEM(world, control_hero, EcsOnUpdate, Vector3, VectorVelocity3, sp_asset_t, tag_hero);
    ecs_set_system_context(world, control_hero, game_context);

    ECS_SYSTEM(world, pre_render, EcsOnUpdate, tag_ui);
    ecs_set_system_context(world, pre_render, game_context);
    ECS_SYSTEM(world, render_ground_tiles, EcsOnUpdate, Vector3, tag_terrain_tile);

    ECS_SYSTEM(world, render_billboards_back, EcsOnUpdate, Vector3, tag_billboard);
    ecs_set_system_context(world, render_billboards_back, game_context);

    ECS_SYSTEM(world, render_bullets, EcsOnUpdate, Vector3, tag_bullet);
    ECS_SYSTEM(world, render_sp_assets, EcsOnUpdate, Vector3, sp_asset_t);

    ECS_SYSTEM(world, render_billboards_front, EcsOnUpdate, Vector3, tag_billboard);
    ecs_set_system_context(world, render_billboards_front, game_context);

    ECS_SYSTEM(world, post_render, EcsOnUpdate, tag_ui);
    ecs_set_system_context(world, post_render, game_context);

    ECS_ENTITY(world, hero_t, Vector3, VectorVelocity3, sp_asset_t, Texture2D, tag_hero);
    ECS_TYPE(world, enemy_t, Vector3, VectorVelocity3, sp_asset_t, tag_enemy);
    ecs_new_w_count(world, enemy_t, 10);

    ECS_TYPE(world, bullet_t, Vector3, VectorVelocity3, VectorAcceleration3, state, tag_bullet);
    ecs_new_w_count(world, bullet_t, 50);

    ECS_TYPE(world, billboard_t, Vector3, tag_billboard);
    ecs_new_w_count(world, billboard_t, 100);

    ECS_TYPE(world, terrain_tile_t, Vector3, tag_terrain_tile);
    ecs_new_w_count(world, terrain_tile_t, 25);

    ecs_new(world, tag_ui);

    ecs_set_target_fps(world, 60);
}

#endif //RAYLIBTEST_GAME_H
