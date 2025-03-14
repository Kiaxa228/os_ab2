#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


typedef struct {
    int** matrix;
    int** result;
    int rows;
    int cols;
    int window_size;
    int start_row;
    int end_row;
} ThreadData;


int getMedian(int* window, int size) {
    int* temp = (int*)malloc(size * sizeof(int));
    memcpy(temp, window, size * sizeof(int));
    
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (temp[j] > temp[j + 1]) {
                int t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }
    
    int median = temp[size / 2];
    free(temp);
    return median;
}

DWORD WINAPI processSection(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    int half_window = data->window_size / 2;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->cols; j++) {
            int window_size = data->window_size * data->window_size;
            int* window = (int*)malloc(window_size * sizeof(int));
            int k = 0;
            
            for (int di = -half_window; di <= half_window; di++) {
                for (int dj = -half_window; dj <= half_window; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    
                    if (ni >= 0 && ni < data->rows && nj >= 0 && nj < data->cols) {
                        window[k++] = data->matrix[ni][nj];
                    } else {
                        window[k++] = 0;
                    }
                }
            }
            
            data->result[i][j] = getMedian(window, window_size);
            free(window);
        }
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <max_threads>\n", argv[0]);
        return 1;
    }
    
    int max_threads = atoi(argv[1]);
    int rows, cols, window_size, k;
    
    printf("Enter matrix rows: ");
    scanf("%d", &rows);
    printf("Enter matrix columns: ");
    scanf("%d", &cols);
    printf("Enter window size (odd number): ");
    scanf("%d", &window_size);
    printf("Enter number of filter applications: ");
    scanf("%d", &k);


    int** matrix = (int**)malloc(rows * sizeof(int*));
    int** temp = (int**)malloc(rows * sizeof(int*));
    int** result = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        temp[i] = (int*)malloc(cols * sizeof(int));
        result[i] = (int*)malloc(cols * sizeof(int));
    }

    printf("Enter matrix elements:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            scanf("%d", &matrix[i][j]);
            temp[i][j] = matrix[i][j];
        }
    }


    for (int iteration = 0; iteration < k; iteration++) {
        int rows_per_thread = rows / max_threads;
        ThreadData* thread_data = (ThreadData*)malloc(max_threads * sizeof(ThreadData));
        HANDLE* threads = (HANDLE*)malloc(max_threads * sizeof(HANDLE));

        for (int t = 0; t < max_threads; t++) {
            thread_data[t].matrix = temp;
            thread_data[t].result = result;
            thread_data[t].rows = rows;
            thread_data[t].cols = cols;
            thread_data[t].window_size = window_size;
            thread_data[t].start_row = t * rows_per_thread;
            thread_data[t].end_row = (t == max_threads - 1) ? rows : (t + 1) * rows_per_thread;

            threads[t] = CreateThread(NULL, 0, processSection, &thread_data[t], 0, NULL);
        }


        WaitForMultipleObjects(max_threads, threads, TRUE, INFINITE);
        for (int t = 0; t < max_threads; t++) {
            CloseHandle(threads[t]);
        }

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                temp[i][j] = result[i][j];
            }
        }

        free(thread_data);
        free(threads);
    }


    printf("\nResult after %d iterations:\n", k);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }


    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
        free(temp[i]);
        free(result[i]);
    }
    free(matrix);
    free(temp);
    free(result);

    return 0;
}