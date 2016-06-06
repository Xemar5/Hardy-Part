#include "Mapping.h"
#include "Input.h"
#include <fstream>
#include <algorithm>

std::map<std::string, std::map<std::string, Input>> Mapping::Maps = {};
std::string Mapping::Default_Path = "";
std::map<std::string, Input> Mapping::Default_Bindings;


void Mapping::Init(std::string path)
{
	Mapping::Default_Path = path;
	Mapping::Default_Bindings =
	{
		{ "a", Input::Set(IT_UNDEFINED) },
		{ "b", Input::Set(IT_UNDEFINED) },
		{ "x", Input::Set(IT_UNDEFINED) },
		{ "y", Input::Set(IT_UNDEFINED) },

		{ "r1", Input::Set(IT_UNDEFINED) },
		{ "r2", Input::Set(IT_UNDEFINED) },
		{ "r3", Input::Set(IT_UNDEFINED) },
		{ "l1", Input::Set(IT_UNDEFINED) },
		{ "l2", Input::Set(IT_UNDEFINED) },
		{ "l3", Input::Set(IT_UNDEFINED) },

		{ "select", Input::Set(IT_UNDEFINED) },
		{ "start", Input::Set(IT_UNDEFINED) },

		{ "laup", Input::Set(IT_UNDEFINED) },
		{ "ladown", Input::Set(IT_UNDEFINED) },
		{ "laleft", Input::Set(IT_UNDEFINED) },
		{ "laright", Input::Set(IT_UNDEFINED) },

		{ "raup", Input::Set(IT_UNDEFINED) },
		{ "radown", Input::Set(IT_UNDEFINED) },
		{ "raleft", Input::Set(IT_UNDEFINED) },
		{ "raright", Input::Set(IT_UNDEFINED) },


		{ "dup", Input::Set(IT_UNDEFINED) },
		{ "ddown", Input::Set(IT_UNDEFINED) },
		{ "dleft", Input::Set(IT_UNDEFINED) },
		{ "dright", Input::Set(IT_UNDEFINED) },
		{ "UNDEFINED", Input::Set(IT_UNDEFINED) }
	};
	Maps = Load_From_File(Mapping::Default_Path);
}

std::string Map_To_String(const std::map<std::string, Input>& map)
{
	std::string str = "";
	for (auto key : map)
	{
		if ((key.second.Type() & 0b111) == IT_UNDEFINED ||
			(key.second.Type() & 0b111) == IT_ANY) continue;
		str += key.first + " = ";
		switch (key.second.Type())
		{
		case IT_GAMEPAD_AXIS: str += "axis "; break;
		case IT_GAMEPAD_BUTTON: str += "button "; break;
		case IT_KEYBOARD_KEY: str += "key "; break;
		case IT_MOUSE_AXIS: str += "mouse "; break;
		case IT_MOUSE_BUTTON: str += "mbutton "; break;
		case IT_GAMEPAD_HAT: str += "hat("; str += std::to_string(Input::Which_Hat(key.second.Key()));  str += ") "; break;
		default: break;
		}
		if (key.second.Type() != IT_GAMEPAD_HAT) str += std::to_string(key.second.Key());
		else
		{
			Uint8 hat = key.second.Key() & 0b1111;
			if		(hat == 0) str += "0";
			else if (hat == 1) str += "1";
			else if (hat == 2) str += "2";
			else if (hat == 3) str += "3";
		}
		str += '\n';
	}
	if (!str.size()) str += "#No Keybinds Set\n";
	return str;
}

bool Mapping::__Save_To_File(const std::string path, const std::map<std::string, std::map<std::string, Input>>& maps)
{
	std::ofstream file;
	file.open(path);
	if (!file)
	{
		file.close();
		return false;
	}

	for (auto map : maps)
	{
		file << '[' << map.first << "]\n";
		file << Map_To_String(map.second);
		file << "\n\n";
	}

	return true;
}

bool Mapping::Read_Line(bool is_action, std::string& src_str, std::string* dst_str, Input* binding)
{
	if (!src_str.size()) return false;
	if (is_action && dst_str && binding)
	{
		if (src_str.find("-1") != std::string::npos) return false;
		src_str.erase(std::remove_if(src_str.begin(), src_str.end(), [](Uint8 c)->bool {return (c == ' ') ? true : false; }), src_str.end());
		std::string action = "";
		Input bdg = Input::Set(IT_UNDEFINED);

		auto act_pos = src_str.find('=');
		if (act_pos == std::string::npos) return false;
		if (Mapping::Default_Bindings.find(src_str.substr(0, act_pos)) == Mapping::Default_Bindings.end()) return false;
		action = src_str.substr(0, act_pos);
		src_str = src_str.substr(act_pos + 1, src_str.length() - act_pos - 1);

		if		(src_str.find("mbutton") != std::string::npos) bdg.__Type = IT_MOUSE_BUTTON;
		else if (src_str.find("button") != std::string::npos) bdg.__Type = (Input_Type)(IT_GAMEPAD_BUTTON | 1<<4);
		else if (src_str.find("axis") != std::string::npos) bdg.__Type = (Input_Type)(IT_GAMEPAD_AXIS | 1 << 4);
		else if (src_str.find("key") != std::string::npos) bdg.__Type = IT_KEYBOARD_KEY;
		else if (src_str.find("mouse") != std::string::npos) bdg.__Type = IT_MOUSE_AXIS;
		else if (src_str.find("hat") != std::string::npos)
		{
			auto bbeg = src_str.find('(');
			auto bend = src_str.find(')');
			if (bbeg == std::string::npos || bend == std::string::npos) return false;

			Uint32 key = 0;
			std::string hat_ind = src_str.substr(bbeg + 1, bend - bbeg - 1);

			hat_ind.erase(std::remove_if(hat_ind.begin(), hat_ind.end(), [](Uint8 c)->bool {return (c >= '0' && c <= '9') ? false : true; }), hat_ind.end());
			if (!hat_ind.size()) return false;
			if (std::atoi(hat_ind.c_str()) > 10) return false;
			key = std::atoi(hat_ind.c_str()) << 4;

			src_str.erase(src_str.begin(), src_str.begin() + bend);
			src_str.erase(std::remove_if(src_str.begin(), src_str.end(), [](Uint8 c)->bool {return (c >= '0' && c <= '9') ? false : true; }), src_str.end());
			if (!src_str.size()) return false;
			if (std::atoi(src_str.c_str()) > 3) return false;
			key = key | (1 << std::atoi(src_str.c_str()));

			bdg.__Type = (Input_Type)(IT_GAMEPAD_HAT | 1 << 4);
			bdg.__Key = Sint32(key);

			*dst_str = action;
			*binding = bdg;
			return true;
		}
		
		src_str.erase(std::remove_if(src_str.begin(), src_str.end(), [](Uint8 c)->bool {return (c >= '0' && c <= '9') ? false : true; }), src_str.end());
		if (!src_str.size()) return false;
		bdg.__Key = std::atoi(src_str.c_str());

		*dst_str = action;
		*binding = bdg;
		return true;
	}
	else if(!is_action && dst_str)
	{
		auto begp = src_str.find('[');
		auto endp = src_str.find(']');
		if (begp == std::string::npos || endp == std::string::npos) return false;
		std::string subs = src_str.substr(begp + 1, endp - begp - 1);
		if (!subs.size()) return false;
		*dst_str = subs;
		return true;
	}
	return false;
}

std::map<std::string, std::map<std::string, Input>> Mapping::Load_From_File(std::string path)
{
	std::ifstream file;
	file.open(path);
	if (!file)
	{
		file.close();
		return{};
	}

	std::map<std::string, std::map<std::string, Input>> v = {};
	std::string str;
	std::string device_name = "";

	while (std::getline(file, str))
	{
		if (!str.size()) continue;
		if (str[0] == '[' && Read_Line(false, str, &device_name))
			if (v.find(device_name) == v.end()) v.insert({ device_name, {} });
		if (str[0] < 'a' || str[0] > 'z') continue;

		Input binding = Input::Set(IT_UNDEFINED);
		std::string action = "UNDEFINED";

		if (device_name.size() && v.find(device_name) != v.end() && Read_Line(true, str, &action, &binding))
			if (action != "UNDEFINED")
				v[device_name].insert({ action, binding });
	}
	return v;
}

bool Mapping::Save(std::string name)
{
	if (Mapping::Maps.find(name) == Mapping::Maps.end())
		Mapping::Maps.insert({ name, __Map });
	else
		for (auto& it : __Map)
		{
			if (it.first == "UNDEFINED") continue;
			Mapping::Maps[name][it.first] = it.second;
		}

	Mapping::__Save_To_File(Mapping::Default_Path, Mapping::Maps);
	return true;
}

Mapping::Mapping(std::map<std::string, Input> map)
{
	__Map = map;
	if (__Map.find("UNDEFINED") == __Map.end()) __Map.insert({ "UNDEFINED", Input::Set(IT_UNDEFINED) });
}
Mapping::Mapping(std::initializer_list<std::pair<std::string const, Input>> list)
{
	__Map = list;
	if (__Map.find("UNDEFINED") == __Map.end()) __Map.insert({ "UNDEFINED", Input::Set(IT_UNDEFINED) });
}

Input Mapping::operator[](std::string name) const
{
	auto it = __Map.find(name);
	if (it != __Map.end()) return it->second;
	return __Map.at("UNDEFINED");
}

Mapping& Mapping::operator=(std::map<std::string, Input> map)
{
	__Map = map;
	if (__Map.find("UNDEFINED") == __Map.end()) __Map.insert({ "UNDEFINED", Input::Set(IT_UNDEFINED) });
	return *this;
}

Mapping & Mapping::operator<<(std::pair<std::string, Input> binding)
{
	auto it = __Map.find(binding.first);
	if (it != __Map.end())
	{
		it->second = binding.second;
		return *this;
	}
	__Map.insert(binding);
	if (__Map.find("UNDEFINED") == __Map.end()) __Map.insert({ "UNDEFINED", Input::Set(IT_UNDEFINED) });
	return *this;
}

std::pair<std::string, Input> operator+(std::string name, const Input& input)
{
	return std::pair<std::string, Input>(name, Input::Set(input.Type(), input.Key()));
}
