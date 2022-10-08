#pragma once

#include <unordered_map>
#include <cassert>

#include "Structs.h"

namespace MH {

class DataStore {
public:
	void InsertSkill(const MH::Skills& arg_skill) {
		assert(_skill_name_map.find(arg_skill.key_name) == _skill_name_map.end());

		_skill_name_map[arg_skill.key_name] = _skills_array.size();
		_skills_array.push_back(arg_skill);
	}

	void InsertArmor(MH::Armor&& arg_armor) {
		assert(_armor_name_map.find(arg_armor.key_name) == _armor_name_map.end());

		_armor_name_map[arg_armor.key_name] = _armors_array.size();
		_armors_array.push_back(std::move(arg_armor));
	}

	void InsertJewel(const MH::Jewel arg_jewel) {
		assert(_jewel_name_map.find(arg_jewel.key_name) == _jewel_name_map.end());

		_jewel_name_map[arg_jewel.key_name] = _jewels_array.size();
		_jewels_array.push_back(arg_jewel);
	}

	size_t GetSkillId(const std::string& arg) {
		assert(_skill_name_map.contains(arg));

		return _skill_name_map[arg];
	}

	std::string_view GetSkillName(size_t id) {
		return _skills_array[id].key_name;
	}

	template<class Stream>
	void DebugPrint(Stream& os) {
		for (auto&& skill : _skill_name_map) {
			os << skill.first << ": " << _skills_array[skill.second].max_level << "\n";
		}
	}

private:
	std::vector<MH::Armor> _armors_array;
	std::vector<MH::Skills> _skills_array;
	std::vector<MH::Jewel> _jewels_array;

	std::unordered_map<std::string, size_t> _armor_name_map;
	std::unordered_map<std::string, size_t> _skill_name_map;
	std::unordered_map<std::string, size_t> _jewel_name_map;
};

} // end of namespace MH