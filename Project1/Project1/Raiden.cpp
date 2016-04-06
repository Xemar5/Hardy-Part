#include "Character.h"

#include "Sprite.h"
#include "Texture.h"
#include "Animation.h"
#include "Movement.h"

void Character<Character_Enum::Raiden>::Create()
{
	Sprite::Create(this, Texture::Load("imgs/raiden-sheet.png", 96, 24, 24, 24, 0, 1));
	Animation::Add(this, "idle", "0", true);
	Animation::Add(this, "move", "0-4x4", false);
	Movement::Set(this, 2.3, 10);
}
void Character<Character_Enum::Raiden>::Update()
{

}
void Character<Character_Enum::Raiden>::Events()
{

}


template class Character<Character_Enum::Raiden>;