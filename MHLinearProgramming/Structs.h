#pragma once

#include <string>
#include <vector>
#include <tuple>

namespace MH {

struct SkillValue {
	size_t id;
	int64_t level;
};

struct Skills {
	std::string key_name;
	int64_t max_level;
};

struct Defenses {
	int64_t defense;
	int64_t fire_res;
	int64_t water_res;
	int64_t ice_res;
	int64_t elec_res;
	int64_t dragon_res;
};

enum class Parts {
	Helm,
	Torso,
	Arm,
	Waist,
	Feet,
};

struct Armor {
	Parts parts;
	std::string key_name;
	std::vector<SkillValue> skills;
	Defenses defense;
	std::tuple<int64_t, int64_t, int64_t> slots;
};

struct Jewel {
	std::string key_name;
	SkillValue skill;
	int64_t slot_size;
};

} // end of namespace MH
