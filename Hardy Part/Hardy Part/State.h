#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "Container.h"
#include "StateContainer.h"

class Tileset;
class Texture;

class State : public StateContainer
{
public:

	//***  Virtual Create function, should be overriden by derivering states
	void Create() override = 0;
	//***  Virtual Update function, should be overriden by derivering states
	void Update() override;
	//***  Virtual Events function, should be overriden by derivering states
	void Events() override;

	//*** If true, System will update all state layers aswell as the main state built before
	//*** Else only this state layer will be updated
	bool Update_Underneath = false;





	//*** Returns the top-most built state
	static std::shared_ptr<State> CurrentState();
	//*** All Built States
	//*** Atleast one of them will get Update'd and Evente'd
	//*** (most of the time the the top State or all of them)
	static std::vector<std::shared_ptr<State>> Built;

	//*** Deleted states in this System loop
	//*** They will be removed at the begining of the next System loop
	static std::vector<unsigned> Deleted;




	//*** Builds and creates a new state of given type
	//*** Deletes all previously built states and state layers
	template <typename T> static std::shared_ptr<T> Change();
	//*** Builds and creates a new state layer of given type
	//*** - update_underneath - if true, System will update all state layers
	//***     aswell as the main state built before
	//*** 	else only this state layer will be updated
	template <typename T> static std::shared_ptr<T> Add(bool update_underneath = false);
	//*** Exits the current state layer and destroys it
	//*** Works only if there is at least one state layer built, prints MSG otherwise
	template <bool = true> static void Remove();
	//*** Exits the game, destroy all states and state layers
	template <bool = true> static void Exit_Game();

	virtual ~State() = 0 {};
private:
	void __Update(Entity<> e);
	void __Events(Entity<> e);

	friend class System;
	friend class Screen;
};









#include "Screen.h"
#include "Output_Handler.h"
#include "Entity.h"
#include "Player.h"
#include "Layer.h"

#include "Menu_State.h"
#include "Main_State.h"
#include "GameMenu_State.h"
#include "Controlls_State.h"




//*** Creates and initializes new State
//*** Template parameter must be supplied with an State derative
//*** Destroys all built State layers and creates new stack
template <typename T>
std::shared_ptr<T> State::Change()
{
	std::cout << "\nChanged into new state\n";
	Device::ClearAllDeviceInput();

	for (unsigned i = 0; i < Built.size(); i++)
		Deleted.push_back(i);

	Built.push_back(std::make_shared<T>());
	Built.back()->Create();
	return std::static_pointer_cast<T>(Built.back());
}


//*** Creates new State layer
//*** Each layer is build on top of an existing one
//*** Layers underneath are not destroyed
//*** - update_underneath - if true, layers underneath will be updated every game tick
template <typename T>
std::shared_ptr<T> State::Add(bool update_underneath)
{
	Device::ClearAllDeviceInput();
	Built.emplace_back(std::make_shared<T>());
	Built.back()->Update_Underneath = update_underneath;
	Built.back()->State::Create();
	Built.back()->Create();
	return std::static_pointer_cast<T>(Built.back());
}

//*** Exits the top State layer and destroys it
template <bool T>
void State::Remove()
{
	Device::ClearAllDeviceInput();
	Deleted.push_back(Built.size() - 1);
}


//*** Exits the game 
template <bool T>
void State::Exit_Game()
{
	Device::ClearAllDeviceInput();
	for (unsigned i = 0; i < Built.size(); i++)
		Deleted.push_back(i);
}

