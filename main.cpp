#include <iostream>
#include <string>
#include "legitimate_process_injector.cpp"

using namespace std;

int main() {
    LegitimateProcessInjector testInjector("D:\\test_path\\test_dll.dll");
    std::string testProcName = "explorer.exe"; 

    if (testInjector.InjectIntoProcess(testProcName) == LegitimateProcessInjector::LegitimateProcessInjectorError::SUCCESS) {
        cout << "Inject true" << endl;
    } else {
        cout << "Inject false" << endl;
    }
    return 0;
}
