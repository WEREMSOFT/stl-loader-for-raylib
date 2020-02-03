//
// Created by Pablo Weremczuk on 2/3/20.
//

#ifndef RAYLIBTEST_BULLETS_H
#define RAYLIBTEST_BULLETS_H

void bullets_init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, VectorAcceleration3, accelerations, 3);

    for (int i = 0; i < rows->count; i++) {
        position[i].z = 0;
        position[i].x = position[i].y = position[i].z = 0;
        velocities[i].x = velocities[i].y = velocities[i].z = 0;
        accelerations[i].x = accelerations[i].z = 0;
        accelerations[i].y = -200;
    }
}

static void bullets_render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for (int i = 0; i < rows->count; i++) {
        DrawCube(position[i], 10.0f, 10.0f, 10.0f, CLITERAL(Color) {11, 110, 176, 255});
        DrawCubeWires(position[i], 10.0f, 10.0f, 10.0f, MAROON);
        Vector3 shadow_position = {position[i].x, 0, position[i].z};
        DrawCube(shadow_position, 10, 1, 10, CLITERAL(Color) {0, 0, 0, 100});
    }
}


void bullets_flying_update(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, VectorAcceleration3, accelerations, 3);
    ECS_COLUMN_COMPONENT(rows, tag_bullet_flying, 4);
    ECS_COLUMN_COMPONENT(rows, tag_bullet_idle, 5);

    for (int i = 0; i < rows->count; i++) {
        Vector3 temp_acceleration = Vector3Multiply(accelerations[i], rows->delta_time);

        velocities[i] = Vector3Add(velocities[i], temp_acceleration);

        Vector3 temp_velocity = Vector3Multiply(velocities[i], rows->delta_time);

        positions[i] = Vector3Add(positions[i], temp_velocity);

        float temp_position_y = positions[i].y;

        positions[i].y = fmax(0, positions[i].y);

        if (temp_position_y != positions[i].y) {
            velocities[i] = Vector3Multiply(velocities[i], 0);
            positions[i] = Vector3Multiply(positions[i], 0);
            ecs_add(rows->world, rows->entities[i], tag_bullet_idle);
            ecs_remove(rows->world, rows->entities[i], tag_bullet_flying);
        }
    }
}

void bullets_idle_update(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Vector3, positions, 1);
    ECS_COLUMN(rows, VectorVelocity3, velocities, 2);
    ECS_COLUMN(rows, VectorAcceleration3, accelerations, 3);
    ECS_COLUMN_COMPONENT(rows, tag_bullet_idle, 4);
    ECS_COLUMN_COMPONENT(rows, tag_bullet_flying, 5);

    bool already_shot = false;

    game_context_t *game_context = ecs_get_system_context(rows->world, rows->system);

    for (int i = 0; i < rows->count; i++) {
        if ((IsKeyPressed(KEY_SPACE) ||
             IsGamepadButtonReleased(GAMEPAD_PLAYER2, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) && !already_shot) {
            already_shot = true;
            float distance_to_target = Vector3Length(game_context->camera.target);
            velocities[i] = Vector3Multiply(Vector3Normalize(game_context->camera.target), 50);
            velocities[i].y = distance_to_target * 2;
            ecs_add(rows->world, rows->entities[i], tag_bullet_flying);
            ecs_remove(rows->world, rows->entities[i], tag_bullet_idle);
        }
    }
}

#endif //RAYLIBTEST_BULLETS_H
