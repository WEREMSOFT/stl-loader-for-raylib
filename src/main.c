/*******************************************************************************************
*
*   raylib example - procedural mesh generation
*
*   This example has been created using raylib 1.8 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (Ray San)
*
********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "stl_loader.h"


#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define PETROL CLITERAL(Color){ 0, 122, 163, 255 }

#if defined(PLATFORM_WEB)
#define GLSL_VERSION            100
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

Camera camera = {{5.0f, 5.0f, 5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};
Vector3 position = {0.0f, 0.0f, 0.0f};
Shader shader = {0};
Model model = {0};

void UpdateDrawFrame(void) {
    UpdateCamera(&camera);
    float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
    SetShaderValue(shader, shader.locs[LOC_VECTOR_VIEW], cameraPos, UNIFORM_VEC3);

    BeginDrawing();{
        ClearBackground(PETROL);

        BeginMode3D(camera);
        {
            DrawModel(model, position, 0.1f, WHITE);
            DrawGrid(10, 1.0);
        } EndMode3D();

        DrawFPS(0, 0);
    } EndDrawing();
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [models] example - stl loading");
    Mesh enano_mesh = load_stl("./assets/enano.stl");

    shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                               FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    model = LoadModelFromMesh(enano_mesh);

    Light light_1 = CreateLight(LIGHT_POINT, (Vector3){ -5, 5, -5 }, Vector3Zero(), RED, shader);
    Light light_2 = CreateLight(LIGHT_POINT, (Vector3){ -5, 5, 5 }, Vector3Zero(), BLUE, shader);
    Light light_3 = CreateLight(LIGHT_POINT, (Vector3){ 5, 5, -5 }, Vector3Zero(), PURPLE, shader);
    UpdateLightValues(shader, light_1);
    UpdateLightValues(shader, light_2);
    UpdateLightValues(shader, light_3);

    SetCameraMode(camera, CAMERA_ORBITAL);
    model.materials[0].shader = shader;

    SetTargetFPS(60);
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif
    UnloadModel(model);
    CloseWindow();

    return 0;
}