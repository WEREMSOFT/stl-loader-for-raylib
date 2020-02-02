//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_ASSETS_H
#define RAYLIBTEST_ASSETS_H

enum SpineAssets {
    DRAGON,
    HERO,
    SPINE_ASSSETS_COUNT,
};

enum Textures {
    TEXTURES_TREE,
    TEXTURES_TREE2,
    TEXTURES_BUSH,
    TEXTURES_HERO,
    TEXTURES_GRASS_1,
    TEXTURES_GRASS_2,
    TEXTURES_GRASS_3,
    TEXTURES_GRASS_4,
    TEXTURES_COUNT
};

sp_asset_t spine_assets[SPINE_ASSSETS_COUNT] = {0};
Texture2D texture_assets[TEXTURES_COUNT] = {0};
// Load shader and set up some uniforms
Shader shader;

static sp_asset_t create_sp_asset(char *json_path, char *atlas_path, char *animation_name) {

    sp_asset_t return_value = {0};

    // Init spine
    return_value.atlas = spAtlas_createFromFile(atlas_path, 0);
    return_value.json = spSkeletonJson_create(return_value.atlas);

    return_value.skeletonData = spSkeletonJson_readSkeletonDataFile(return_value.json, json_path);

    if (!return_value.skeletonData) {
        printf("%s\n", return_value.json->error);
        spSkeletonJson_dispose(return_value.json);
        printf("ERROR!\n");
    }

    spBone_setYDown(true);
    return_value.skeleton = spSkeleton_create(return_value.skeletonData);
    return_value.skeleton->scaleX = 0.5;
    return_value.skeleton->scaleY = -0.5;

    // Create the spAnimationStateData
    return_value.animationStateData = spAnimationStateData_create(return_value.skeletonData);
    return_value.animationState = spAnimationState_create(return_value.animationStateData);
    // Add the animation "walk" to track 0, without delay, and let it loop indefinitely
    int track = 0;
    int loop = 1;
    float delay = 0;
    spAnimationState_addAnimationByName(return_value.animationState, track, animation_name, loop, delay);
    spAnimationState_addAnimationByName(return_value.animationState, 0, animation_name, 1, 0);
    spAnimationState_update(return_value.animationState, .0f);
    spAnimationState_apply(return_value.animationState, return_value.skeleton);
    spSkeleton_updateWorldTransform(return_value.skeleton);

    return return_value;
}

void init_assets() {
//    spine_assets[DRAGON] = create_sp_asset("assets/dragon/NewDragon.json","assets/dragon/NewDragon.atlas", "flying");
    spine_assets[HERO] = create_sp_asset("assets/hero/hero.json","assets/hero/hero.atlas", "run");
    spine_assets[DRAGON] = create_sp_asset("assets/enemy/enemy.json","assets/enemy/enemy.atlas", "animation");
    texture_assets[TEXTURES_TREE] = LoadTexture("assets/images/treee.png");
    texture_assets[TEXTURES_TREE2] = LoadTexture("assets/images/tree2.png");
    texture_assets[TEXTURES_BUSH] = LoadTexture("assets/images/bush.png");
    texture_assets[TEXTURES_HERO] = LoadTexture("assets/images/hero1.png");
    texture_assets[TEXTURES_GRASS_1] = LoadTexture("assets/images/grass1.png");
    texture_assets[TEXTURES_GRASS_2] = LoadTexture("assets/images/grass2.png");
    texture_assets[TEXTURES_GRASS_3] = LoadTexture("assets/images/grass3.png");
    texture_assets[TEXTURES_GRASS_4] = LoadTexture("assets/images/grass4.png");

    shader = LoadShader("assets/shaders/fog.vs", "assets/shaders/fog.fs");
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
}

void destroy_assets() {
    for (int i = 0; i < SPINE_ASSSETS_COUNT; i++) {
        spAtlas_dispose(spine_assets[i].atlas);
        spSkeleton_dispose(spine_assets[i].skeleton);
    }
    texture_2d_destroy(); // Destroy textures loaded by spine
    for(int i = 0; i < TEXTURES_COUNT; i++){
        UnloadTexture(texture_assets[i]);
    }
    UnloadShader(shader);
}

#endif //RAYLIBTEST_ASSETS_H
