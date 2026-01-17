#include "audioPlayer/Audio_System.h"
#include "audioPlayer/Sound.h"
#include <iostream>

void clearConsoleScreen()
{
    COORD tl = { 0, 0 };
    CONSOLE_SCREEN_BUFFER_INFO s;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &s);
    DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    FillConsoleOutputCharacter(console, ' ', cells, tl, &written);
    FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(console, tl);
}

const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& getReverbFromID(int id)
{
    int trueID = id % 30;

    switch (trueID)
    {
    case 0:
        return ReverbParameters::Default;
    case 1:
        return ReverbParameters::Generic;
    case 2:
        return ReverbParameters::Paddedcell;
    case 3:
        return ReverbParameters::Room;
    case 4:
        return ReverbParameters::Bathroom;
    case 5:
        return ReverbParameters::Livingroom;
    case 6:
        return ReverbParameters::Stoneroom;
    case 7:
        return ReverbParameters::Auditorium;
    case 8:
        return ReverbParameters::Concerthall;
    case 9:
        return ReverbParameters::Cave;
    case 10:
        return ReverbParameters::Arena;
    case 11:
        return ReverbParameters::Hangar;
    case 12:
        return ReverbParameters::Carpetedhallway;
    case 13:
        return ReverbParameters::Hallway;
    case 14:
        return ReverbParameters::Stonecorridor;
    case 15:
        return ReverbParameters::Alley;
    case 16:
        return ReverbParameters::Forest;
    case 17:
        return ReverbParameters::City;
    case 18:
        return ReverbParameters::Mountains;
    case 19:
        return ReverbParameters::Quarry;
    case 20:
        return ReverbParameters::Plain;
    case 21:
        return ReverbParameters::Parkinglot;
    case 22:
        return ReverbParameters::Sewerpipe;
    case 23:
        return ReverbParameters::Underwater;
    case 24:
        return ReverbParameters::Smallroom;
    case 25:
        return ReverbParameters::Mediumroom;
    case 26:
        return ReverbParameters::Largeroom;
    case 27:
        return ReverbParameters::Mediumhall;
    case 28:
        return ReverbParameters::Largehall;
    case 29:
        return ReverbParameters::Plate;
    }

    return ReverbParameters::Default;
}

std::string nameOfReverbFromID(int id)
{
    int trueID = id % 30;

    switch (trueID)
    {
    case 0:
        return "Default / none";
    case 1:
        return "Generic";
    case 2:
        return "Padded cell";
    case 3:
        return "Room";
    case 4:
        return "Bathroom";
    case 5:
        return "Living room";
    case 6:
        return "Stone room";
    case 7:
        return "Auditorium";
    case 8:
        return "Concert hall";
    case 9:
        return "Cave";
    case 10:
        return "Arena";
    case 11:
        return "Hangar";
    case 12:
        return "Carpeted hallway";
    case 13:
        return "Hallway";
    case 14:
        return "Stone corridor";
    case 15:
        return "Alley";
    case 16:
        return "Forest";
    case 17:
        return "City";
    case 18:
        return "Mountains";
    case 19:
        return "Quarry";
    case 20:
        return "Plain";
    case 21:
        return "Parking lot";
    case 22:
        return "Sewer pipe";
    case 23:
        return "Underwater";
    case 24:
        return "Small room";
    case 25:
        return "Medium room";
    case 26:
        return "Larger room";
    case 27:
        return "Medium hall";
    case 28:
        return "Large hall";
    case 29:
        return "Plate";
    }

    return "Default / none";
}

bool controls(Audio_System& _audioSystem, Sound& Emitter, bool& quit, int& m_reverb)
{
    bool change = false;

    if (GetKeyState(VK_ESCAPE) & 0x8000)
    {
        quit = true;
        return false;
    }

    // Position controls
    {
        float addX = 0.f;
        float addY = 0.f;
        float addZ = 0.f;

        //auto actual = Audio_System::getListenerForwad();

        if (GetKeyState('A') & 0x8000)
        {
            addX -= 1;
        }

        if (GetKeyState('W') & 0x8000)
        {
            addY -= 1;
        }

        if (GetKeyState('D') & 0x8000)
        {
            addX += 1;
        }

        if (GetKeyState('S') & 0x8000)
        {
            addY += 1;
        }

        if (addX != 0 || addY != 0 || addZ != 0)
        {
            _audioSystem.moveListenerPosition(addX, addY, addZ);
            change = true;
        }
    }

    // Orientation controls
    {
        if (GetKeyState(VK_LEFT) & 0x8000)
        {
            _audioSystem.setListenerForward(1, 0, 0);
            change = true;
        }

        if (GetKeyState(VK_UP) & 0x8000)
        {
            _audioSystem.setListenerForward(0, 1, 0);
            change = true;
        }

        if (GetKeyState(VK_RIGHT) & 0x8000)
        {
            _audioSystem.setListenerForward(-1, 0, 0);
            change = true;
        }

        if (GetKeyState(VK_DOWN) & 0x8000)
        {
            _audioSystem.setListenerForward(0, -1, 0);
            change = true;
        }
    }

    // Reverb controls
    {
        if (GetKeyState(VK_ADD) & 0x8000)
        {
            if (m_reverb < MAXINT)
            {
                Emitter.setReverb(getReverbFromID(++m_reverb));
            }
            change = true;
        }

        if (GetKeyState(VK_SUBTRACT) & 0x8000)
        {
            if (m_reverb > 0)
            {
                Emitter.setReverb(getReverbFromID(--m_reverb));
            }
            change = true;
        }
    }

    return change;
}

void printInfos(const Audio_System& _audioSystem, const Sound& Emitter, int m_reverb)
{
    clearConsoleScreen();

    for (int y = 0; y < 25; y++)
    {
        if (y == 20)
        {
            std::cout << "Y\t\t";
        }
        else if (y == 21)
        {
            std::cout << "^\t\t";
        }
        else if (y == 22 || y == 23)
        {
            std::cout << "|\t\t";
        }
        else if (y == 24)
        {
            std::cout << " ---> X\t\t";
        }
        else
        {
            std::cout << "\t\t";
        }

        for (int x = 0; x < 25; x++)
        {
            if (_audioSystem.getListenerPosition().x == x && _audioSystem.getListenerPosition().y == y)
            {
                auto forward = _audioSystem.getListenerForward();

                if (forward.x == 1 && forward.y == 0)
                {
                    std::cout << "< ";
                }
                else if (forward.x == 0 && forward.y == 1)
                {
                    std::cout << "^ ";
                }
                else if (forward.x == -1 && forward.y == 0)
                {
                    std::cout << "> ";
                }
                else if (forward.x == 0 && forward.y == -1)
                {
                    std::cout << "v ";
                }
                else
                {
                    std::cout << "? ";
                }
            }
            else if (Emitter.getPosition().x == x && Emitter.getPosition().y == y)
            {
                std::cout << "o ";
            }
            else
            {
                std::cout << ". ";
            }
        }

        if (y == 0)
        {
            std::cout << "\t\t\t Position ";
        }
        else if (y == 1)
        {
            std::cout << "\t\t\t x : " << _audioSystem.getListenerPosition().x;
        }
        else if (y == 2)
        {
            std::cout << "\t\t\t y : " << _audioSystem.getListenerPosition().y;
        }

        else if (y == 4)
        {
            std::cout << "\t\t\t Forward ";
        }
        else if (y == 5)
        {
            std::cout << "\t\t\t x : " << _audioSystem.getListenerForward().x;
        }
        else if (y == 6)
        {
            std::cout << "\t\t\t y : " << _audioSystem.getListenerForward().y;
        }
        else if (y == 7)
        {
            std::cout << "\t\t\t z : " << _audioSystem.getListenerForward().z;
        }

        else if (y == 9)
        {
            std::cout << "\t\t\t Misc ";
        }
        else if (y == 10)
        {
            std::cout << "\t\t\t Left : " << Emitter.getDSPInfos().pMatrixCoefficients[0];
        }
        else if (y == 11)
        {
            std::cout << "\t\t\t Right : " << Emitter.getDSPInfos().pMatrixCoefficients[1];
        }
        else if (y == 12)
        {
            std::cout << "\t\t\t reverb type : " << nameOfReverbFromID(m_reverb);
        }
        else if (y == 13)
        {
            std::cout << "\t\t\t reverb Level : " << Emitter.getDSPInfos().ReverbLevel;
        }

        else if (y == 15)
        {
            std::cout << "\t\t\t Controls ";
        }
        else if (y == 16)
        {
            std::cout << "\t\t\t Move : w a s d keys";
        }
        else if (y == 17)
        {
            std::cout << "\t\t\t Orientation : Arrow keys";
        }
        else if (y == 18)
        {
            std::cout << "\t\t\t Reverb : Add / Subtract";
        }

        std::cout << "\n";
    }
}

void example(std::string SoundPath)
{
    Audio_System audioSystem;

    // set max hearing distance (for scaling).
    audioSystem.setDefaultMaxDistance(12.f); // ToDo: change in data file

    // set the listener position.
    audioSystem.setListenerPosition(12, 15, 0);

    // set the forward to y.
    // as this program run in 2D we set the Top of our listener on the axis we don't use (Z axis in this case).
    // the top is principality use for knowing with ear is left and right.
    audioSystem.setListenerOrientation({ 0, 1, 0 }, { 0, 0, -1 });

    // load our Sound
    Sound Emitter(SoundPath);

    // check if it is well loaded but not needed, here we use a TTC instead.
    //if (!Emitter.isLinked()) {
    //	return -1;
    //}

    // set our sound a the middle, this enable 3D specialization automatically.
    Emitter.setPosition(12, 12, 0);

    // set the sound looping and play it.
    Emitter.setLooping(true);
    Emitter.play();

    int reverbID = 0;
    bool Program_exit = false;

    // console output.
    printInfos(audioSystem, Emitter, reverbID);

    while (!Program_exit)
    {
        if (controls(audioSystem, Emitter, Program_exit, reverbID))
        {
            printInfos(audioSystem, Emitter, reverbID);
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
    }
}