// MHLinearProgramming.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <cassert>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>
#include <chrono>

#include <simdjson.h>

#include "DataStore.h"
#include "Structs.h"


constexpr char json_path[] = "C:/Users/elenesgu/repos/MHLinearProgramming/mhrise_calculator/crawler/data";

int main()
{
    auto now = std::chrono::high_resolution_clock::now;

    auto start = now();

    MH::DataStore data_store;
    std::filesystem::path base_path = json_path;

    auto skill_path = base_path / "skill.json";
    simdjson::ondemand::parser parser;
    auto&& skill_json = simdjson::padded_string::load(skill_path.generic_string());
    auto&& skill_doc = parser.iterate(skill_json);

    auto&& skill_array = skill_doc.get_array();

    for (auto&& skill : skill_array) {
        auto&& skill_obj = skill.get_object();
        std::string_view id = skill_obj["id"];
        int64_t max_level = skill_obj["maxLevel"].get_int64();

        data_store.InsertSkill({{ id.begin(), id.end() }, max_level});
    }

    // data_store.DebugPrint(std::cout);

    static const auto CheckArmorType = [](auto view) {
        using namespace std::literals;

        if (view == "helm"sv) {
            return MH::Parts::Helm;
        }
        if (view == "torso"sv) {
            return MH::Parts::Torso;
        }
        if (view == "arm"sv) {
            return MH::Parts::Arm;
        }
        if (view == "waist"sv) {
            return MH::Parts::Waist;
        }
        if (view == "feet"sv) {
            return MH::Parts::Feet;
        }

        throw std::logic_error("Not reachable");
    };

    auto armor_path = base_path / "armor.json";

    auto&& armor_json = simdjson::padded_string::load(armor_path.generic_string());
    auto&& armor_doc = parser.iterate(armor_json);

    auto&& armor_arr = armor_doc.get_array();
    for (auto&& armor : armor_arr) {
        auto&& armor_obj = armor.get_object();
        
        std::string_view id = armor_obj["id"];
        std::string_view type = armor_obj["part"];
        
        auto&& defense_stat = armor_obj["stat"].get_object();

        int64_t phy_def    = defense_stat["defense"].get_int64();
        int64_t fire_res   = defense_stat["fireRes"].get_int64();
        int64_t water_res  = defense_stat["waterRes"].get_int64();
        int64_t ice_res    = defense_stat["iceRes"].get_int64();
        int64_t elec_res   = defense_stat["elecRes"].get_int64();
        int64_t dragon_res = defense_stat["dragonRes"].get_int64();

        MH::Armor armor_v{
            .parts{CheckArmorType(type)},
            .key_name{id.begin(), id.end()},
            .defense{phy_def, fire_res, water_res, ice_res, elec_res, dragon_res}
        };

        auto&& armor_skills = armor_obj["skills"].get_object();

        for (auto&& armor_skill_item : armor_skills) {
            std::string_view key_name = armor_skill_item.unescaped_key();
            int64_t skill_level = armor_skill_item.value()["level"].get_int64();

            armor_v.skills.emplace_back(data_store.GetSkillId({ key_name.begin(), key_name.end()}), skill_level);
        }

        auto&& slots = armor_obj["slots"].get_array();
        assert(slots.count_elements() == 3);

        auto slot_itr = slots.begin();
        std::get<0>(armor_v.slots) = (*slot_itr).get_int64(); ++slot_itr;
        std::get<1>(armor_v.slots) = (*slot_itr).get_int64(); ++slot_itr;
        std::get<2>(armor_v.slots) = (*slot_itr).get_int64();

        data_store.InsertArmor(std::move(armor_v));
    }

    auto jewel_path = base_path / "deco.json";

    auto&& jewel_json = simdjson::padded_string::load(jewel_path.generic_string());
    auto&& jewel_doc = parser.iterate(jewel_json);

    auto&& jewel_arr = jewel_doc.get_array();
    for (auto&& jewel : jewel_arr) {
        auto jewel_obj = jewel.get_object();

        std::string_view id = jewel_obj["id"];
        std::string_view skill = jewel_obj["skillId"];

        int64_t skill_level = jewel_obj["skillLevel"].get_int64();
        int64_t slot_size = jewel_obj["slotSize"].get_int64();

        data_store.InsertJewel({
            {id.begin(), id.end()},
            {data_store.GetSkillId({skill.begin(), skill.end()}), skill_level},
            slot_size
        });
    }

    auto end = now();

    auto collapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << collapsed / 1000 << "s " << collapsed % 1000 << "ms" << std::endl;

    return 0;
}