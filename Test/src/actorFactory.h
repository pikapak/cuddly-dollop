#ifndef ACTOR_FACTORY_H__
#define ACTOR_FACTORY_H__

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include "utils.h"
#include "actor.h"
#include "skill.h"

class ActorFactory
{
public:
	static Actor_ptr BuildBaseAlly();
	static Actor_ptr BuildBaseEnemy();
	static std::vector<Actor_ptr> BuildParty(rapidjson::GenericArray<false, rapidjson::Value>& arr);
	static Actor_ptr BuildActor(rapidjson::Value& a);
	static Actor_ptr BuildActor(int id, int team = 1, int level = 1, int fluctuation = 3);
	static Skill_ptr BuildSkill(std::string name);
};

#endif // !EVENT_FACTORY_H__

