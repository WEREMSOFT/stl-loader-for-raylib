//
// Created by Pablo Weremczuk on 2/3/20.
//

#ifndef RAYLIBTEST_SPINE_UTILS_H
#define RAYLIBTEST_SPINE_UTILS_H
#include "../utils/structs.h"
#include <spine-raylib.h>

void sp_set_animation(sp_asset_t *sp_asset, char *animation_name) {
    int track = 0;
    int loop = 1;

    spTrackEntry *te = spAnimationState_getCurrent(sp_asset->animationState, track);

    if (strncmp(animation_name, te->animation->name, 30) != 0) {
        spAnimationState_setAnimationByName(sp_asset->animationState, track, animation_name, loop);
    }
}
#endif //RAYLIBTEST_SPINE_UTILS_H
