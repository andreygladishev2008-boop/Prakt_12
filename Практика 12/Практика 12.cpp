// Практика 12.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// Сделал сам, вариант 3

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <Windows.h>

using namespace std;

const int MAX_PROCESSES = 100;

void runVariant(int from, int to);

struct ProcessInfo
{
    string name;
    string command;
    DWORD pid;
    HANDLE handle;
};

ProcessInfo processes[MAX_PROCESSES];
int processCount = 0;

bool isRunning(ProcessInfo& p)
{
    DWORD code = 0;
    if (GetExitCodeProcess(p.handle, &code))
    {
        return code == STILL_ACTIVE;
    }
    return false;
}

bool startProcess(const string& name, const string& command)
{
    if (processCount >= MAX_PROCESSES)
    {
        cout << "  Too many processes" << endl;
        return false;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    char buffer[1024];
    strcpy_s(buffer, 1024, command.c_str());

    BOOL created = CreateProcessA(NULL, buffer, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    if (!created)
    {
        DWORD error = GetLastError();
        cout << "  Error! Cannot create process. Code: " << error << endl;
        if (error == 2)
        {
            cout << "  (file not found)" << endl;
        }
        return false;
    }

    processes[processCount].name = name;
    processes[processCount].command = command;
    processes[processCount].pid = pi.dwProcessId;
    processes[processCount].handle = pi.hProcess;
    processCount++;

    CloseHandle(pi.hThread);

    cout << "  Process started: " << name << ", ID = " << processes[processCount - 1].pid << endl;
    return true;
}

void showList()
{
    if (processCount == 0)
    {
        cout << "   List is empty" << endl;
        return;
    }

    cout << "\n" << endl;
    for (int i = 0; i < processCount; i++)
    {
        string state = isRunning(processes[i]) ? "working" : "finished";
        cout << "  " << i + 1 << ") " << processes[i].name
            << ", ID = " << processes[i].pid
            << ", state: " << state << endl;
        cout << "     command: " << processes[i].command << endl;
    }
    cout << "\n" << endl;
}

int countRunning()
{
    int count = 0;
    for (int i = 0; i < processCount; i++)
    {
        if (isRunning(processes[i]))
        {
            count++;
        }
    }
    return count;
}

void killProcess()
{
    showList();
    if (processCount == 0)
    {
        return;
    }

    cout << "   Enter process number to kill: ";
    int number = 0;
    cin >> number;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "  Wrong input" << endl;
        return;
    }

    if (number < 1 || number > processCount)
    {
        cout << "   No such number" << endl;
        return;
    }

    ProcessInfo& p = processes[number - 1];

    if (!isRunning(p))
    {
        cout << "   This process is already finished" << endl;
        return;
    }

    if (TerminateProcess(p.handle, 0))
    {
        cout << "   Process " << p.name << " (ID = " << p.pid << ") was killed" << endl;
    }
    else
    {
        cout << "   Cannot kill process. Code: " << GetLastError() << endl;
    }
}

void startPing()
{
    string address = "";
    int count = 4;

    cout << "   Address (for example 8.8.8.8): ";
    cin >> address;
    cout << "   How many requests (0 = work forever): ";
    cin >> count;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "   Wrong input" << endl;
        return;
    }

    string command = "ping.exe ";
    if (count == 0)
    {
        command = command + "-t ";
    }
    else
    {
        command = command + "-n " + to_string(count) + " ";
    }
    command = command + address;

    startProcess("ping.exe", command);
}

void startVariant()
{
    int from = 0;
    int to = 0;

    cout << "   Range start: ";
    cin >> from;
    cout << "   Range end: ";
    cin >> to;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "   Wrong input" << endl;
        return;
    }

    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    string command = string("\"") + path + "\" calc " + to_string(from) + " " + to_string(to);

    startProcess("Our program (variant 3)", command);
}

void runVariant(int from, int to)
{
    if (from > to)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    cout << "Variant 3. Numbers from " << from << " to " << to << endl;
    cout << "Number\tSum of digits" << endl;

    int bestNumber = from;
    int bestSum = -1;

    for (int i = from; i <= to; i++)
    {
        int number = i;
        if (number < 0)
        {
            number = -number;
        }

        int sum = 0;
        while (number > 0)
        {
            sum = sum + number % 10;
            number = number / 10;
        }

        cout << i << "\t" << sum << endl;

        if (sum > bestSum)
        {
            bestSum = sum;
            bestNumber = i;
        }
    }

    cout << "Max sum of digits = " << bestSum << " for number " << bestNumber << endl;

    system("pause");
}

int main(int argc, char* argv[])
{
    if (argc == 4 && string(argv[1]) == "calc")
    {
        int from = atoi(argv[2]);
        int to = atoi(argv[3]);
        runVariant(from, to);
        return 0;
    }

    cout << "   Process manager" << endl;

    int choice = -1;
    while (choice != 0)
    {
        cout << endl;
        cout << "1. start taskmgr.exe" << endl;
        cout << "2. start ping.exe" << endl;
        cout << "3. start program of variant 3" << endl;
        cout << "4. show list of processes" << endl;
        cout << "5. show number of working processes" << endl;
        cout << "6. kill process" << endl;
        cout << "0. exit" << endl;
        cout << "Your choice: ";
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
            continue;
        }

        if (choice == 1)
        {
            startProcess("taskmgr.exe", "taskmgr.exe");
        }
        else if (choice == 2)
        {
            startPing();
        }
        else if (choice == 3)
        {
            startVariant();
        }
        else if (choice == 4)
        {
            showList();
        }
        else if (choice == 5)
        {
            cout << "  Working processes: " << countRunning() << endl;
        }
        else if (choice == 6)
        {
            killProcess();
        }
        else if (choice != 0)
        {
            cout << "  No such item" << endl;
        }
    }

    char killAll = 'n';
    if (countRunning() > 0)
    {
        cout << "  Kill all working processes before exit? (y/n): ";
        cin >> killAll;
    }

    for (int i = 0; i < processCount; i++)
    {
        if (isRunning(processes[i]) && (killAll == 'y' || killAll == 'Y'))
        {
            TerminateProcess(processes[i].handle, 0);
        }
        CloseHandle(processes[i].handle);
    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
