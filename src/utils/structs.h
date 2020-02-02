//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_STRUCTS_H
#define RAYLIBTEST_STRUCTS_H

enum Sreens {
    SCREEN_MAIN_MENU,
    SCREEN_TUTORIAL,
    SCREEN_GAME,
    SCREEN_COUNT
};

enum enum_asset_type {
    ASSET_TYPE_HERO,
    ASSET_TYPE_ENEMY,
    ASSET_TYPE_TREE_1,
    ASSET_TYPE_TREE_2,
    ASSET_TYPE_BUSH,
    ASSET_TYPE_COUNT
};

typedef struct game_context_t {
    Vector2 screen_size;
    Camera3D camera;
    ecs_world_t *world;
} game_context_t;

typedef struct rendereable_t {
    Vector3 position;
    void *asset;
    int asset_type;
} rendereable_t;

typedef struct sp_asset_t {
    spAtlas *atlas;
    spSkeletonJson *json;
    spSkeletonData *skeletonData;
    spSkeleton *skeleton;
    spAnimationStateData *animationStateData;
    spAnimationState *animationState;
} sp_asset_t;

#endif //RAYLIBTEST_STRUCTS_H
