#include <Windows.h>
#include <iostream>
#include <thread>

void example(std::string SoundPath);
void tests(std::string SoundPath);

int main(int argc, char** argv)
try
{
    std::string SoundPath;

    // can load song by draggin a song file on the exe file.
    if (argc < 2)
    {
        std::cout << "\nDrag a sound file on the executable.\n\n";
        std::cout << "Press escape to leave.\n\n\n";

        while (!(GetKeyState(VK_ESCAPE) & 0x8000))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    else
    {
        SoundPath = argv[1];

        std::cout << "\nHello there !\n\n";
        std::cout << "Press 0 to enter tests.\n";
        std::cout << "Press 1 to enter exemple.\n\n";
        std::cout << "Press escape to leave.\n\n\n";

        while (!(GetKeyState(VK_ESCAPE) & 0x8000))
        {
            if ((GetKeyState('0') & 0x8000) || (GetKeyState(VK_NUMPAD0) & 0x8000))
            {
                tests(SoundPath);
                break;
            }
            else if ((GetKeyState('1') & 0x8000) || (GetKeyState(VK_NUMPAD1) & 0x8000))
            {
                example(SoundPath);
                break;
            }
        }
    }
}
catch (const std::exception& e)
{
    std::cerr << "\n\n"
              << e.what() << "\n\n";
    std::cout << "press escape to quit\n";

    while (!(GetKeyState(VK_ESCAPE) & 0x8000))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return -1;
}
catch (...)
{
    std::cerr << "\n\n"
              << "Unknown Error"
              << "\n\n";
    std::cout << "press escape to quit\n";

    while (!(GetKeyState(VK_ESCAPE) & 0x8000))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return -1;
}