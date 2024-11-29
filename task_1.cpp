#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <aclapi.h>

void printFileAttributes(DWORD attributes) {
    if (attributes & FILE_ATTRIBUTE_ARCHIVE) std::cout << "Archive ";
    if (attributes & FILE_ATTRIBUTE_COMPRESSED) std::cout << "Compressed ";
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) std::cout << "Directory ";
    if (attributes & FILE_ATTRIBUTE_ENCRYPTED) std::cout << "Encrypted ";
    if (attributes & FILE_ATTRIBUTE_HIDDEN) std::cout << "Hidden ";
    if (attributes & FILE_ATTRIBUTE_NORMAL) std::cout << "Normal ";
    if (attributes & FILE_ATTRIBUTE_READONLY) std::cout << "Read-Only ";
    if (attributes & FILE_ATTRIBUTE_SYSTEM) std::cout << "System ";
    if (attributes & FILE_ATTRIBUTE_TEMPORARY) std::cout << "Temporary ";
    std::cout << std::endl;
}

void printFileTime(const FILETIME& fileTime) {
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&fileTime, &systemTime);
    std::cout << systemTime.wDay << "/"
              << systemTime.wMonth << "/"
              << systemTime.wYear << " "
              << systemTime.wHour << ":"
              << systemTime.wMinute << ":"
              << systemTime.wSecond << std::endl;
}

void getFileOwner(const std::string& filePath) {
    PSECURITY_DESCRIPTOR securityDescriptor;
    DWORD ownerSize;
    PSID ownerSID;
    LPSTR ownerName;
    DWORD ownerNameSize = 100;
    LPSTR domainName;
    DWORD domainNameSize = 100;
    SID_NAME_USE sidNameUse;

    // Get security information
    if (GetSecurityInfo(
        CreateFileA(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL),
        SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
        &ownerSID, NULL, NULL, NULL, &securityDescriptor) == ERROR_SUCCESS) {

        ownerName = (LPSTR)malloc(ownerNameSize);
        domainName = (LPSTR)malloc(domainNameSize);

        if (LookupAccountSidA(NULL, ownerSID, ownerName, &ownerNameSize, domainName, &domainNameSize, &sidNameUse)) {
            std::cout << "Owner: " << domainName << "\\" << ownerName << std::endl;
        } else {
            std::cout << "Unable to get owner." << std::endl;
        }

        free(ownerName);
        free(domainName);
    } else {
        std::cout << "Unable to get security information." << std::endl;
    }
}

int main() {
    std::string filePath;
    std::cout << "Enter the path of the file: ";
    std::cin >> filePath;

    // Get file attributes
    DWORD fileAttributes = GetFileAttributes(filePath.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Error: Unable to retrieve file attributes." << std::endl;
        return 1;
    }

    std::cout << "File Attributes: ";
    printFileAttributes(fileAttributes);

    // Get file size
    HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return 1;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        std::cerr << "Error: Unable to retrieve file size." << std::endl;
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "File Size: " << fileSize << " bytes" << std::endl;

    // Get file creation, last access, and last write time
    FILETIME creationTime, lastAccessTime, lastWriteTime;
    if (!GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime)) {
        std::cerr << "Error: Unable to retrieve file times." << std::endl;
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "Creation Time: ";
    printFileTime(creationTime);

    std::cout << "Last Access Time: ";
    printFileTime(lastAccessTime);

    std::cout << "Last Write Time: ";
    printFileTime(lastWriteTime);

    // Get file owner
    getFileOwner(filePath);

    // Close file handle
    CloseHandle(hFile);

    return 0;
}
