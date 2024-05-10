#include "solution.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG_MODE 1  // Set to 1 to enable debug prints, 0 to disable

#if DEBUG_MODE
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

zs_lock lab_lock;

void *scientist_routine(void *arg) {
    int id = *(int *)arg;
    DEBUG_PRINT("Scientist %d attempting to enter the lab.\n", id);
    if (scientist_enter(&lab_lock) == 0) {
        DEBUG_PRINT("Scientist %d entered the lab.\n", id);
        sleep(1); // Simulate work inside the lab
        if (scientist_exit(&lab_lock) == 0) {
            DEBUG_PRINT("Scientist %d exited the lab.\n", id);
        } else {
            DEBUG_PRINT("Error: Scientist %d could not exit the lab.\n", id);
        }
    } else {
        DEBUG_PRINT("Error: Scientist %d could not enter the lab.\n", id);
    }
    return NULL;
}

void *zombie_routine(void *arg) {
    int id = *(int *)arg;
    DEBUG_PRINT("Zombie %d attempting to enter the lab.\n", id);
    if (zombie_enter(&lab_lock) == 0) {
        DEBUG_PRINT("Zombie %d entered the lab.\n", id);
        sleep(1); // Simulate time spent inside the lab
        if (zombie_exit(&lab_lock) == 0) {
            DEBUG_PRINT("Zombie %d exited the lab.\n", id);
        } else {
            DEBUG_PRINT("Error: Zombie %d could not exit the lab.\n", id);
        }
    } else {
        DEBUG_PRINT("Error: Zombie %d could not enter the lab.\n", id);
    }
    return NULL;
}

int main() {
    pthread_t threads[6];
    int ids[6] = {1, 2, 3, 4, 5, 6};
    // Initialize the lab lock
    lab_lock.lock = (spinlock_t *)malloc(sizeof(spinlock_t));

    if (lab_lock.lock == NULL) {
        exit(EXIT_FAILURE);
    }

    spinlock_init_ece391(lab_lock.lock);
    lab_lock.num_z = 0;
    lab_lock.num_s = 0;
    lab_lock.num_z_wait = 0;
    lab_lock.num_s_wait = 0;
    int i;
    // Create threads for scientists and zombies
    for (i = 0; i < 4; ++i) { // 4 scientists
        pthread_create(&threads[i], NULL, scientist_routine, &ids[i]);
    }
    for (i = 4; i < 6; ++i) { // 2 zombies
        pthread_create(&threads[i], NULL, zombie_routine, &ids[i]);
    }
    // printf("pthreads create\n");
    // Wait for all threads to complete
    for (i = 0; i < 6; ++i) {
        pthread_join(threads[i], NULL);
    }
    // printf("good job!\n");
    // Cleanup lab lock
    spinlock_destroy_ece391(lab_lock.lock);
    return 0;
}
