#pragma once

enum class Wall_Enum
{
	Cobble
};

#include "Entity.h"

template <Wall_Enum type>
class Wall : public Entity
{
public:
	Wall_Enum Type = type;
	void Create() override;
	void Update() override;
	void Events() override;
};