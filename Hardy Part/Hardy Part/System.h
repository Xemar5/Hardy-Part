#pragma once
#include <SDL.h>
#include <SDL_net.h>
#include <vector>
#include <memory>
#include <stdio.h>
#include "Audio.h"

class Timer;
class Object;

class System final
{
public:
	//*** Initialises the system with given State
	template <typename T>
	static void Start();

	//*** SDL Event handler
	static SDL_Event Events;
	//*** The timer that resets everytime the frame should be updated
	//*** Adjust the FPS to make it reset faster/slower
	static Timer FPS_Clock;
	//*** The timer that starts at the beginning of the program
	static Timer In_Game_Timer;
	//*** The approximate number of frames per second
	static unsigned FPS;

protected:

	//*** Main System loop, where all the states are updated
	static void _System_Update();
private:
	//*** Iteratees through all children of the state and deletes them
	//*** - sets child's parent to nullptr
	//*** - if child is of Container type, iterates through its children
	static void __ClearChildren(std::shared_ptr<Object> ent);

	static void __Delete();
	static void __Update();
	static void __Events();
	
};

#include <SDL_ttf.h>
#include "Output_Handler.h"
#include "Screen.h"
#include "Mapping.h"
//#include "Network.h"

template <typename T>
void System::Start()
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
	{
		Output_Handler::Error << "\nUnable to initialize SDL:" << SDL_GetError() << "\n";
	}
	if (TTF_Init() != 0)
	{
		Output_Handler::Error << "\nUnable to initialize TTF:" << TTF_GetError() << "\n";
	}
	srand(time(nullptr));
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
	controlls::Mapping::Init("keybinds.txt");
	Screen::Init();
	Audio::Init();
	//Network::Init();
	State::Change<T>();
	//Audio::Play_Music(Audio::Load_Music("imgs/Baker Cat.mp3"));
	//Screen::Change_Window_State();
	_System_Update();
}
