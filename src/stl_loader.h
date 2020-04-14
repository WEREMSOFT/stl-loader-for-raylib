//
// Created by Pablo Weremczuk on 4/15/20.
//

#ifndef RAYLIB_FLECS_SPINE_STL_LOADER_H
#define RAYLIB_FLECS_SPINE_STL_LOADER_H

#include <stdio.h>

#ifndef __uint8_t
    typedef unsigned char __uint8_t;
#endif

#ifndef __uint16_t
    typedef unsigned short __uint16_t;
#endif

#pragma pack(push, 1)
typedef struct vertex_info_t {
    Vector3 normal;
    Vector3 triangle[3];
    __uint16_t attribute;
} vertex_info_t;
#pragma pack(pop)

Mesh load_stl(char *file_path) {
    Mesh mesh = {0};

    mesh.vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));

    FILE *fap = fopen(file_path, "r");


    if(fap == NULL){
        perror("File not found");
        exit(-1);
    }

    unsigned int triangle_count = 0;

    fseek(fap, sizeof(__uint8_t) * 80, 0);
    fread(&triangle_count, sizeof(int), 1, fap);
    vertex_info_t *model_info = (vertex_info_t *) RL_MALLOC(triangle_count * sizeof(vertex_info_t));

    if(model_info == NULL) {
        perror("Error creating model");
        exit(-1);
    }

    mesh.vertexCount = triangle_count * 3;
    mesh.triangleCount = triangle_count;
    mesh.normals = (float *)RL_MALLOC(sizeof(Vector3) * triangle_count * 3);
    mesh.vertices = (float *)RL_MALLOC(sizeof(Vector3) * triangle_count * 3);
    mesh.texcoords = (float *)RL_MALLOC(sizeof(Vector3) * triangle_count * 3);


    size_t registers_read = fread(model_info, sizeof(vertex_info_t), triangle_count, fap);

    if(registers_read < triangle_count) {
        printf("Error. Unable to read the expected number of triangles: %zu out of %zu", registers_read, triangle_count);
        exit(-1);
    } else {
        printf("%zu triangles read\n", registers_read);
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

    rlLoadMesh(&mesh, false);
    RL_FREE(model_info);
    fclose(fap);
    return mesh;
}


#endif //RAYLIB_FLECS_SPINE_STL_LOADER_H
