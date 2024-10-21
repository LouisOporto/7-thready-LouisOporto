#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Implement three patterns of threads using threads
// 1. Main calls all threads
// 2. Recursive call of threads
// 3. Heap-like call of threads

// Each process goals:
// Print process beginning
// Print process making a process(if it does)
// Print process exiting
// Try to add the PID in this calls
typedef struct {
    int iter;
    int size;
    long threadPID;
} Data;

void* iterativeThreads(void* vptr);
void* recursiveThread(void* vptr);

int main(int argc, char* argv[]) {
    int pattern;
    int size;
    srand(0);

    if (argc < 3 || sscanf(argv[1], "%d", &size) == 0 ||
        sscanf(argv[2], "%d", &pattern) == 0) {
        printf("Invaild input: ./forky.exe <size> <patternNum>\n");
        return (1);
    }
    // Pattern 1
    if (pattern == 1) {
        // printf("Pattern 1\n");
        pthread_t* threadArray = (pthread_t*)malloc(sizeof(pthread_t) * size);
        Data* dataArray = (Data*)malloc(sizeof(Data) * size);

        for (int iter = 0; iter < size; iter++) {
            dataArray[iter].iter = iter;

            if (pthread_create(threadArray + iter, NULL, iterativeThreads,
                               &dataArray[iter]) != 0) {
                perror("Fail to create thread");
                return 1;
            }
        }

        for (int iter = 0; iter < size; iter++) {
            if (pthread_join(threadArray[iter], NULL) != 0) {
                return 2;
            }
        }

        free(threadArray);
        free(dataArray);
    }

    // Pattern 2
    if (pattern == 2) {
        // printf("Pattern 2\n");
        pthread_t initalThread;
        Data data = {0, size, 0};

        if (data.iter < size) {
            if (pthread_create(&initalThread, NULL, recursiveThread, &data) !=
                0) {
                perror("Fail to create thread");
                return 1;
            }
        }

        pthread_join(initalThread, NULL);
    }

    /* OPTIONAL */
    if (pattern == 3) {
        // printf("Pattern 3\n");
    }

    return 0;
}

void* iterativeThreads(void* vptr) {
    Data* data = (Data*)vptr;
    fprintf(stdout, "Process child thread %d (%ld)\n", data->iter + 1,
            pthread_self());
    sleep(rand() % 8);
    fprintf(stdout, "Exiting child thread %d (%ld)\n", data->iter + 1,
            pthread_self());
    return NULL;
}

void* recursiveThread(void* vptr) {
    Data* data = (Data*)vptr;
    int currentIter = data->iter;
    long prevPID = data->threadPID;
    data->threadPID = pthread_self();

    if (data->iter < data->size) {
        pthread_t newThread;
        data->iter += 1;
        if(currentIter == 0) {
            fprintf(stdout, "Process started Thread %d (%ld)\n", currentIter + 1, pthread_self());
        } else {
            fprintf(stdout, "Thread %d (%ld) started Thread %d (%ld)\n", currentIter, prevPID,  currentIter + 1, data->threadPID);
        }

        fprintf(stdout, "Thread %d beginnging (%ld)\n", currentIter + 1, data->threadPID);

        if (pthread_create(&newThread, NULL, recursiveThread, data) != 0) {
            perror("Failed to creat thread");
            return NULL;
        }

        sleep(rand() % 4);  // Sleep for 0 - 2 seconds;
        pthread_join(newThread, NULL);
        fprintf(stdout, "Thread %d exiting (%ld)\n", currentIter + 1, data->threadPID);
    }

    return NULL;
}