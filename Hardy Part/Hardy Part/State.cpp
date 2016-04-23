#include "State.h"
#include "Entity.h"
#include "Screen.h"
#include "Sprite.h"
#include "Animation.h"
#include "Movement.h"
#include "Tileset.h"

std::vector<std::shared_ptr<State>> State::Built;
std::vector<unsigned> State::Deleted;


void State::Update()
{
	//for (auto tileset : __Tilesets)
	//	Screen::Add(tileset.get());
	for (auto layer : Layers)
		if (layer->Update_Entities)
			for (auto ent : layer->Entities)
			{
				if (Animation::Next_Frame(ent.get()) != -1)
					Animation::Set_Frame(ent.get(), ent->Get_Sprite()->Get_Current_Frame());
				
				ent->Update();

				if (ent->Get_Movement())
					if (Movement::__Resolve_Movement(ent.get()))
						Animation::Play(ent.get(), "move");
					else Animation::Terminate(ent.get(), "move");

				if (!ent->Get_Sprite()->Get_Current_Animation()) Animation::Play(ent.get());
				//Screen::Add(ent.get());
			}
	Screen::Draw();
}
void State::Events()
{
	for (auto layer : Layers)
		if (layer->Events_Entities)
			for (auto ent : layer->Entities)
				ent->Events();
}

bool State::Add_Tileset(std::shared_ptr<Texture> texture, std::pair<int, int> pos, std::vector<std::vector<unsigned>> map)
{
	if (!this)
	{
		std::cerr << "ERR State::Add_Tileset : No this State\n";
		return false;
	}
	if (!texture)
	{
		std::cerr << "ERR State::Add_Tileset : No texture supplied\n";
		return false;
	}
	__Tilesets.push_back(Tileset::Set(texture, pos, map));
	Screen::Add(__Tilesets.back());
	return true;
}

std::vector<std::shared_ptr<Tileset>> State::Get_Tilesets()
{
	return __Tilesets;
}

Layer & State::operator[](unsigned layer)
{
	if (layer >= Layers.size())
	{
		std::cerr << "ERR State::operator[] : State has no layer with given index; creating layer with given index and belove\n";
		while (Layers.size() <= layer) Layers.emplace_back(std::make_shared<Layer>());
	}
	return *Layers[layer].get();
}

Entity * Layer::operator[](unsigned ent)
{
	if (ent >= Entities.size())
	{
		std::cerr << "ERR Layer::operator[] : Layer has no entity with given index\n";
		return nullptr;
	}
	return Entities[ent].get();
}
