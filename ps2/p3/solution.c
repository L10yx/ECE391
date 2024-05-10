#include <stdio.h>
#include "solution.h"

int zombie_enter(zs_lock* zs) {
    spinlock_lock_ece391(zs->lock);
    zs->num_z_wait++;
    while (zs->num_s || (zs->num_s_wait && !zs->num_z) || zs->num_z >= 10) {
        spinlock_unlock_ece391(zs->lock);
        spinlock_lock_ece391(zs->lock);
    }
    zs->num_z_wait--, zs->num_z++;
    // printf("zombia enter\n");
    spinlock_unlock_ece391(zs->lock);
    return 0;
}

int zombie_exit(zs_lock* zs) {
    spinlock_lock_ece391(zs->lock);
    zs->num_z--;
    // printf("zombia leave\n");
    spinlock_unlock_ece391(zs->lock);
    return 0;
}

int scientist_enter(zs_lock* zs) {
    spinlock_lock_ece391(zs->lock);
    zs->num_s_wait++;
    while (zs->num_z || zs->num_s >= 4) {
        spinlock_unlock_ece391(zs->lock);
        spinlock_lock_ece391(zs->lock);
    }
    zs->num_s_wait--, zs->num_s++;
    // printf("scientist enter\n");
    spinlock_unlock_ece391(zs->lock);
    return 0;
}

int scientist_exit(zs_lock* zs) {
    spinlock_lock_ece391(zs->lock);
    zs->num_s--;
    // printf("scientist leave\n");
    spinlock_unlock_ece391(zs->lock);
    return 0;
}


