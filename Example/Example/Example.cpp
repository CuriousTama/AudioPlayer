#include "Audio/Sound.h"
#include <iostream>

void clear_screen() {
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, ' ', cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}

const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& getReverbFromID(int id) {
	int trueID = id % 30;

	switch (trueID)
	{
	case 0: return Sound::Reverb::Default; break;
	case 1: return Sound::Reverb::Generic; break;
	case 2: return Sound::Reverb::Paddedcell; break;
	case 3: return Sound::Reverb::Room; break;
	case 4: return Sound::Reverb::Bathroom; break;
	case 5: return Sound::Reverb::Livingroom; break;
	case 6: return Sound::Reverb::Stoneroom; break;
	case 7: return Sound::Reverb::Auditorium; break;
	case 8: return Sound::Reverb::Concerthall; break;
	case 9: return Sound::Reverb::Cave; break;
	case 10: return Sound::Reverb::Arena; break;
	case 11: return Sound::Reverb::Hangar; break;
	case 12: return Sound::Reverb::Carpetedhallway; break;
	case 13: return Sound::Reverb::Hallway; break;
	case 14: return Sound::Reverb::Stonecorridor; break;
	case 15: return Sound::Reverb::Alley; break;
	case 16: return Sound::Reverb::Forest; break;
	case 17: return Sound::Reverb::City; break;
	case 18: return Sound::Reverb::Mountains; break;
	case 19: return Sound::Reverb::Quarry; break;
	case 20: return Sound::Reverb::Plain; break;
	case 21: return Sound::Reverb::Parkinglot; break;
	case 22: return Sound::Reverb::Sewerpipe; break;
	case 23: return Sound::Reverb::Underwater; break;
	case 24: return Sound::Reverb::Smallroom; break;
	case 25: return Sound::Reverb::Mediumroom; break;
	case 26: return Sound::Reverb::Largeroom; break;
	case 27: return Sound::Reverb::Mediumhall; break;
	case 28: return Sound::Reverb::Largehall; break;
	case 29: return Sound::Reverb::Plate; break;
	}

	return Sound::Reverb::Default;
}

std::string nameOfReverbFromID(int id) {
	int trueID = id % 30;

	switch (trueID)
	{
	case 0: return "Default / none"; break;
	case 1: return "Generic"; break;
	case 2: return "Padded cell"; break;
	case 3: return "Room"; break;
	case 4: return "Bathroom"; break;
	case 5: return "Living room"; break;
	case 6: return "Stone room"; break;
	case 7: return "Auditorium"; break;
	case 8: return "Concert hall"; break;
	case 9: return "Cave"; break;
	case 10: return "Arena"; break;
	case 11: return "Hangar"; break;
	case 12: return "Carpeted hallway"; break;
	case 13: return "Hallway"; break;
	case 14: return "Stone corridor"; break;
	case 15: return "Alley"; break;
	case 16: return "Forest"; break;
	case 17: return "City"; break;
	case 18: return "Mountains"; break;
	case 19: return "Quarry"; break;
	case 20: return "Plain"; break;
	case 21: return "Parking lot"; break;
	case 22: return "Sewer pipe"; break;
	case 23: return "Underwater"; break;
	case 24: return "Small room"; break;
	case 25: return "Medium room"; break;
	case 26: return "Larger room"; break;
	case 27: return "Medium hall"; break;
	case 28: return "Large hall"; break;
	case 29: return "Plate"; break;
	}

	return "Default / none";
}

bool Controles(Sound& Emitter, bool& quit, int& m_reverb) {
	bool change = false;

	if (GetKeyState(VK_ESCAPE) & 0x8000) {
		quit = true;
		return false;
	}

	// Position controles
	{
		float addX = 0.f;
		float addY = 0.f;
		float addZ = 0.f;

		//auto actual = Audio_System::getListenerForwad();

		if (GetKeyState('A') & 0x8000) {
			addX -= 1;
		}

		if (GetKeyState('W') & 0x8000) {
			addY -= 1;
		}

		if (GetKeyState('D') & 0x8000) {
			addX += 1;
		}

		if (GetKeyState('S') & 0x8000) {
			addY += 1;
		}

		if (addX != 0 || addY != 0 || addZ != 0) {
			Audio_System::moveListenerPosition(addX, addY, addZ);
			change = true;
		}
	}

	// Orientation Controles
	{
		if (GetKeyState(VK_LEFT) & 0x8000) {
			Audio_System::setListenerForward(1, 0, 0);
			change = true;
		}

		if (GetKeyState(VK_UP) & 0x8000) {
			Audio_System::setListenerForward(0, 1, 0);
			change = true;
		}

		if (GetKeyState(VK_RIGHT) & 0x8000) {
			Audio_System::setListenerForward(-1, 0, 0);
			change = true;
		}

		if (GetKeyState(VK_DOWN) & 0x8000) {
			Audio_System::setListenerForward(0, -1, 0);
			change = true;
		}
	}

	// Reverb Controles
	{
		if (GetKeyState(VK_ADD) & 0x8000) {
			if (m_reverb < MAXINT) {
				Emitter.setReverb(getReverbFromID(++m_reverb));
			}
			change = true;
		}

		if (GetKeyState(VK_SUBTRACT) & 0x8000) {
			if (m_reverb > 0) {
				Emitter.setReverb(getReverbFromID(--m_reverb));
			}
			change = true;
		}
	}

	return change;
}

void PrintInfos(Sound& Emitter, const int m_reverb) {
	clear_screen();

	for (int y = 0; y < 25; y++) {

		if (y == 20) {
			std::cout << "Y\t\t";
		}
		else if (y == 21) {
			std::cout << "^\t\t";
		}
		else if (y == 22 || y == 23) {
			std::cout << "|\t\t";
		}
		else if (y == 24) {
			std::cout << " ---> X\t\t";
		}
		else {
			std::cout << "\t\t";
		}


		for (int x = 0; x < 25; x++) {

			if (Audio_System::getListenerPosition().x == x && Audio_System::getListenerPosition().y == y) {
				auto forward = Audio_System::getListenerForwad();

				if (forward.x == 1 && forward.y == 0) {
					std::cout << "< ";
				}
				else if (forward.x == 0 && forward.y == 1) {
					std::cout << "^ ";
				}
				else if (forward.x == -1 && forward.y == 0) {
					std::cout << "> ";
				}
				else if (forward.x == 0 && forward.y == -1) {
					std::cout << "v ";
				}
				else {
					std::cout << "? ";
				}

			}
			else if (Emitter.getPosition().x == x && Emitter.getPosition().y == y) {
				std::cout << "o ";
			}
			else {
				std::cout << ". ";
			}
		}


		if (y == 0) {
			std::cout << "\t\t\t Position ";
		}
		else if (y == 1) {
			std::cout << "\t\t\t x : " << Audio_System::getListenerPosition().x;
		}
		else if (y == 2) {
			std::cout << "\t\t\t y : " << Audio_System::getListenerPosition().y;
		}

		else if (y == 4) {
			std::cout << "\t\t\t Forward ";
		}
		else if (y == 5) {
			std::cout << "\t\t\t x : " << Audio_System::getListenerForwad().x;
		}
		else if (y == 6) {
			std::cout << "\t\t\t y : " << Audio_System::getListenerForwad().y;
		}
		else if (y == 7) {
			std::cout << "\t\t\t z : " << Audio_System::getListenerForwad().z;
		}

		else if (y == 9) {
			std::cout << "\t\t\t Misc ";
		}
		else if (y == 10) {
			std::cout << "\t\t\t Left : " << Emitter.getDSPInfos().pMatrixCoefficients[0];
		}
		else if (y == 11) {
			std::cout << "\t\t\t Right : " << Emitter.getDSPInfos().pMatrixCoefficients[1];
		}
		else if (y == 12) {
			std::cout << "\t\t\t reverb type : " << nameOfReverbFromID(m_reverb);
		}
		else if (y == 13) {
			std::cout << "\t\t\t reverb Level : " << Emitter.getDSPInfos().ReverbLevel;
		}

		else if (y == 15) {
			std::cout << "\t\t\t Controles ";
		}
		else if (y == 16) {
			std::cout << "\t\t\t Move : w a s d keys";
		}
		else if (y == 17) {
			std::cout << "\t\t\t Orientation : Arrow keys";
		}
		else if (y == 18) {
			std::cout << "\t\t\t Reverb : Add / Subtract";
		}

		std::cout << "\n";
	}
}

void example(std::string SoundPath)
{
	int reverbID = 0;
	bool Program_exit = false;

	// set max hearing distance (for scaling).
	Audio_System::setDefaultMaxDistance(12.f);

	// set the listener position.
	Audio_System::setListenerPosition(12, 15, 0);

	// set the forward to y.
	// as this program run in 2D we set the Top of our listener on the axis we don't use (Z axis in this case).
	// the top is principality use for knowing with ear is leftand right.
	Audio_System::setListenerOrientation({ 0, 1, 0 }, { 0, 0, -1 });

	// load our Sound
	Sound Emitter(SoundPath);

	// check if it is well loaded but not needed, here we use a TTC insteed.
	//if (!Emitter.isLinked()) {
	//	return -1;
	//}

	// set our sound a the middle, this enable 3D spacialization automatically.
	Emitter.setPosition(12, 12, 0);

	// set the sound looping and play it.
	Emitter.setLooping(true);
	Emitter.play();

	// console output.
	PrintInfos(Emitter, reverbID);

	while (!Program_exit) {
		if (Controles(Emitter, Program_exit, reverbID)) {
			PrintInfos(Emitter, reverbID);
			std::this_thread::sleep_for(std::chrono::milliseconds(60));
		}
	}
}