#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 1024 * 1024 // 1 MB

void readFileStandard(const char *filePath) {
    FILE *file = fopen(filePath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        // Симулюємо обробку даних
        // fwrite(buffer, 1, bytesRead, stdout); // Для виведення на екран
    }

    fclose(file);
}

int main() {
    const char *filePath = "large_file.dat"; // Замініть на шлях до вашого файлу

    clock_t start = clock();
    readFileStandard(filePath);
    clock_t end = clock();

    double elapsedTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken (Standard C functions): %.6f seconds\n", elapsedTime);

    return 0;
}
