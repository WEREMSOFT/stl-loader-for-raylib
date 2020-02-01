//
// Created by Pablo Weremczuk on 1/31/20.
//

#ifndef RAYLIBTEST_ASSETS_H
#define RAYLIBTEST_ASSETS_H

enum SpineAssets {
    HERO,
    SPINE_ASSSETS_COUNT,
    DRAGON
};

sp_asset_t spine_assets[SPINE_ASSSETS_COUNT] = {0};

static sp_asset_t sp_asset_create_dragon(char *json_path, char *atlas_path, char *animation_name) {

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
    return_value.skeleton->scaleX = 0.1;
    return_value.skeleton->scaleY = 0.1;

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
//    spine_assets[DRAGON] = sp_asset_create_dragon("assets/dragon/NewDragon.json","assets/dragon/NewDragon.atlas", "flying");
    spine_assets[HERO] = sp_asset_create_dragon("assets/hero/hero.json","assets/hero/hero.atlas", "run");
}

void destroy_assets() {
    for (int i = 0; i < SPINE_ASSSETS_COUNT; i++) {
        spAtlas_dispose(spine_assets[i].atlas);
        spSkeleton_dispose(spine_assets[i].skeleton);
    }
    texture_2d_destroy(); // Destroy textures loaded by spine
}

#endif //RAYLIBTEST_ASSETS_H
