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

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define PETROL CLITERAL(Color){ 0, 122, 163, 255 }

#if defined(PLATFORM_WEB)
typedef unsigned char           __uint8_t;
typedef unsigned short          __uint16_t;
#define GLSL_VERSION            100
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            330
#endif

#pragma pack(1)
typedef struct vertex_info_t {
    Vector3 normal;
    Vector3 triangle[3];
    __uint16_t attribute;
} vertex_info_t;

Mesh load_stl(char *file_path) {
    Mesh mesh = {0};

    printf("tamano %d\n", sizeof(vertex_info_t));

    mesh.vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));

    FILE *fap = fopen(file_path, "r");


    if(fap == NULL){
        perror("no encontre el archivo man");
        exit(-1);
    }

    unsigned int triangle_count = 0;

    fseek(fap, sizeof(__uint8_t) * 80, 0);
    fread(&triangle_count, sizeof(int), 1, fap);
    vertex_info_t *model_info = (vertex_info_t *) RL_MALLOC(triangle_count * sizeof(vertex_info_t));

    if(model_info == NULL) {
        perror("NAAAAA");
        exit(-1);
    }

    mesh.vertexCount = triangle_count * 3;
    mesh.triangleCount = triangle_count;
    mesh.normals = (float *)RL_MALLOC(sizeof(Vector3) * triangle_count * 3);
    mesh.vertices = (float *)RL_MALLOC(sizeof(Vector3) * triangle_count * 3);
    mesh.texcoords = (float *)RL_MALLOC(sizeof(Vector3) * triangle_count * 3);


    size_t registers_read = fread(model_info, sizeof(vertex_info_t), triangle_count, fap);

    if(registers_read < triangle_count) {
        printf("error. No se pudieron leer la cantidad de objetos esperado: %d de %d", registers_read, triangle_count);
        exit(-1);
    } else {
        printf("Leidos %d triangulos\n", registers_read);
    }

    for (int i = 0, t = 0; t < triangle_count; t++) {
        for(int vertex_index = 0; vertex_index < 3; vertex_index++){
            mesh.vertices[i++] = model_info[t].triangle[vertex_index].x;
            mesh.vertices[i++] = model_info[t].triangle[vertex_index].y;
            mesh.vertices[i++] = model_info[t].triangle[vertex_index].z;
        }
    }

    for (int i = 0, t = 0; t < triangle_count; t++) {
        for(int j = 0; j < 3; j++){
            mesh.normals[i++] = model_info[t].normal.x;
            mesh.normals[i++] = model_info[t].normal.y;
            mesh.normals[i++] = model_info[t].normal.z;
        }
    }

    // Upload vertex data to GPU (static mesh)
    rlLoadMesh(&mesh, false);
    RL_FREE(model_info);
    fclose(fap);
    return mesh;
}

// Define the camera to look into our 3d world
Camera camera = {{5.0f, 5.0f, 5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};
// Model drawing position
Vector3 position = {0.0f, 0.0f, 0.0f};
Shader shader = {0};
Model model = {0};

void UpdateDrawFrame(void) {
    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera);      // Update internal camera and our camera
    float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
    SetShaderValue(shader, shader.locs[LOC_VECTOR_VIEW], cameraPos, UNIFORM_VEC3);

    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();{
        ClearBackground(PETROL);

        BeginMode3D(camera);
        {
            DrawModel(model, position, 0.1f, WHITE);
            DrawGrid(10, 1.0);
        } EndMode3D();

        DrawFPS(0, 0);
    } EndDrawing();
    //----------------------------------------------------------------------------------
}

int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitWindow(screenWidth, screenHeight, "raylib [models] example - mesh generation");
    Mesh enano_mesh = load_stl("./assets/enano.stl");

    shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                               FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    // Get some shader loactions
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    model = LoadModelFromMesh(enano_mesh);

    // Set checked texture as default diffuse component for all models material

    Light light_1 = CreateLight(LIGHT_POINT, (Vector3){ -5, 5, -5 }, Vector3Zero(), RED, shader);
    Light light_2 = CreateLight(LIGHT_POINT, (Vector3){ -5, 5, 5 }, Vector3Zero(), BLUE, shader);
    Light light_3 = CreateLight(LIGHT_POINT, (Vector3){ 5, 5, -5 }, Vector3Zero(), PURPLE, shader);
    UpdateLightValues(shader, light_1);
    UpdateLightValues(shader, light_2);
    UpdateLightValues(shader, light_3);




    int currentModel = 0;

    SetCameraMode(camera, CAMERA_ORBITAL);  // Set a orbital camera mode
    model.materials[0].shader = shader;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------

    // Unload models data (GPU VRAM)
    UnloadModel(model);
    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}