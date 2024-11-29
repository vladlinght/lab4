#include <windows.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 1024 * 1024 // 1 MB

void readFileWindowsAPI(const char *filePath) {
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Error opening file\n");
        return;
    }

    DWORD bytesRead;
    char *buffer = (char *)malloc(BUFFER_SIZE);

    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        CloseHandle(hFile);
        return;
    }

    while (ReadFile(hFile, buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        // Симулюємо обробку даних
        // WriteFile(stdout, buffer, bytesRead, &bytesWritten, NULL); // Для виведення на екран
    }

    free(buffer);
    CloseHandle(hFile);
}

int main() {
    const char *filePath = "large_file.dat"; // Замініть на шлях до вашого файлу

    clock_t start = clock();
    readFileWindowsAPI(filePath);
    clock_t end = clock();

    double elapsedTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken (Windows API - unbuffered): %.6f seconds\n", elapsedTime);

    return 0;
}
