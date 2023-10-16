#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
void displayProcesses() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cout << "Помилка." << std::endl;
        return;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        std::cout << "Помилка." << std::endl;
        CloseHandle(hProcessSnap);
        return;
    }
    int count = 1;
    do {
        std::wcout << count << ". " << pe32.szExeFile << " (PID: " << pe32.th32ProcessID << ")" << std::endl;
        count++;
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

void displayProcessDetails(DWORD processID) {
    HANDLE hProcess;
    FILETIME createTime, exitTime, kernelTime, userTime;
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess != NULL) {
        GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime);
        std::cout << "PID: " << processID << std::endl;
        std::cout << "Час початку: " << createTime.dwLowDateTime << std::endl;
        std::cout << "Загальний час процесора: " << kernelTime.dwLowDateTime + userTime.dwLowDateTime << std::endl;
        CloseHandle(hProcess);
    }
    else {
        std::cout << "Помилка." << std::endl;
    }
}

void killProcess(DWORD processID) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    if (hProcess != NULL) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        std::cout << "Успішно закритий." << std::endl;
    }
    else {
        std::cout << "Помилка." << std::endl;
    }
}

int main() {
    system("chcp 1251>nul");
    std::cout << "Список процесів: " << std::endl;
    displayProcesses();
    int choice;
    std::cout << "Номер процеса для вибору: ";
    std::cin >> choice;
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);
    int count = 1;
    DWORD selectedProcessID;
    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (count == choice) {
                selectedProcessID = pe32.th32ProcessID;
                break;
            }
            count++;
        } while (Process32Next(hProcessSnap, &pe32));
    }

    displayProcessDetails(selectedProcessID);

    char terminateChoice;
    std::cout << "Припинити процес? (y/n): ";
    std::cin >> terminateChoice;
    if (terminateChoice == 'y') {
        killProcess(selectedProcessID);
    }
    //displayProcesses();
    return 0;
}
