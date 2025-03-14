#include <iostream>
#include <string>
#include "legitimate_process_injector.cpp"

using namespace std;

int main() {
    LegitimateProcessInjector testInjector("D:\\Visual\\MyProg\\Malicious_code_into_legitimate_processes\\test_dll.dll");
    std::string testProcName = "explorer.exe"; 

    if (testInjector.InjectIntoProcess(testProcName) == LegitimateProcessInjector::LegitimateProcessInjectorError::SUCCESS) {
        cout << "Inject true" << endl;
    } else {
        cout << "Inject false" << endl;
    }
    return 0;
}