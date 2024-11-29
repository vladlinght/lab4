#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

#define BUFFER_SIZE 1024 // Розмір буфера

// Структура для зберігання інформації про файл
struct AsyncFile {
    HANDLE hFile;
    OVERLAPPED overlapped;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    DWORD bytesWritten;
    HANDLE hEvent;
};

void readFileAsync(AsyncFile& file) {
    file.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  // Створення події для асинхронної операції
    if (file.hEvent == NULL) {
        std::cerr << "Error creating event" << std::endl;
        return;
    }

    // ініціалізація структури OVERLAPPED
    ZeroMemory(&file.overlapped, sizeof(OVERLAPPED));
    file.overlapped.hEvent = file.hEvent;

    // Читання файлу асинхронно
    if (!ReadFile(file.hFile, file.buffer, BUFFER_SIZE, &file.bytesRead, &file.overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            std::cerr << "ReadFile failed with error: " << GetLastError() << std::endl;
        }
    }
}

void writeFileAsync(AsyncFile& file) {
    file.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  // Створення події для асинхронної операції
    if (file.hEvent == NULL) {
        std::cerr << "Error creating event" << std::endl;
        return;
    }

    // ініціалізація структури OVERLAPPED
    ZeroMemory(&file.overlapped, sizeof(OVERLAPPED));
    file.overlapped.hEvent = file.hEvent;

    // Запис файлу асинхронно
    if (!WriteFile(file.hFile, file.buffer, file.bytesRead, &file.bytesWritten, &file.overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            std::cerr << "WriteFile failed with error: " << GetLastError() << std::endl;
        }
    }
}

void processFiles(const std::vector<std::string>& filePaths) {
    std::vector<AsyncFile> asyncFiles(filePaths.size());

    // Відкриваємо файли асинхронно
    for (size_t i = 0; i < filePaths.size(); ++i) {
        asyncFiles[i].hFile = CreateFile(filePaths[i].c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (asyncFiles[i].hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening file: " << filePaths[i] << std::endl;
            continue;
        }

        // Читання файлів асинхронно
        readFileAsync(asyncFiles[i]);
    }

    // Очікуємо завершення всіх асинхронних операцій
    std::vector<HANDLE> events(filePaths.size());
    for (size_t i = 0; i < filePaths.size(); ++i) {
        events[i] = asyncFiles[i].hEvent;
    }

    DWORD dwResult = WaitForMultipleObjects(events.size(), events.data(), TRUE, INFINITE);
    if (dwResult == WAIT_FAILED) {
        std::cerr << "WaitForMultipleObjects failed with error: " << GetLastError() << std::endl;
        return;
    }

    // Перевіряємо результати операцій
    for (size_t i = 0; i < filePaths.size(); ++i) {
        if (!GetOverlappedResult(asyncFiles[i].hFile, &asyncFiles[i].overlapped, &asyncFiles[i].bytesRead, TRUE)) {
            std::cerr << "GetOverlappedResult failed for file: " << filePaths[i] << std::endl;
            continue;
        }

        std::cout << "Read " << asyncFiles[i].bytesRead << " bytes from file: " << filePaths[i] << std::endl;

        // Тепер записуємо в файл
        writeFileAsync(asyncFiles[i]);
    }

    // Чекаємо завершення запису
    dwResult = WaitForMultipleObjects(events.size(), events.data(), TRUE, INFINITE);
    if (dwResult == WAIT_FAILED) {
        std::cerr << "WaitForMultipleObjects failed with error: " << GetLastError() << std::endl;
        return;
    }

    // Перевіряємо результати запису
    for (size_t i = 0; i < filePaths.size(); ++i) {
        if (!GetOverlappedResult(asyncFiles[i].hFile, &asyncFiles[i].overlapped, &asyncFiles[i].bytesWritten, TRUE)) {
            std::cerr << "GetOverlappedResult failed for write operation on file: " << filePaths[i] << std::endl;
            continue;
        }

        std::cout << "Written " << asyncFiles[i].bytesWritten << " bytes to file: " << filePaths[i] << std::endl;

        // Закриваємо файл
        CloseHandle(asyncFiles[i].hFile);
    }
}

int main() {
    std::vector<std::string> filePaths = { "file1.dat", "file2.dat", "file3.dat" };

    processFiles(filePaths);

    return 0;
}
