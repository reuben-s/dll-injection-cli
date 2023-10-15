#include <string>
#include "Injector.h">

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Missing arguments." << std::endl;
        return 1;
    }

    DWORD dwPid      = std::stoul(argv[1]);
    LPCSTR lpDllPath = argv[2];

    InjectDLL dll_injector(dwPid, lpDllPath);
    try
    {
        dll_injector.Inject();
    }
    catch (const std::exception& e)
    {
        LPVOID lpvErrorMessage;
        DWORD size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            GetLastError(),
            0,
            (LPWSTR)&lpvErrorMessage,
            0,
            NULL
        );
        std::wcout << e.what() << static_cast<LPCWSTR>(lpvErrorMessage) << std::endl;
        LocalFree(lpvErrorMessage);
    }
}