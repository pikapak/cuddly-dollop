#ifndef PASSIVE_FACTORY_H__
#define PASSIVE_FACTORY_H__

#include "fighter.h"
#include "battleData.h"

class PassiveFactory
{
public:
	// Make sure to reset the stats before calling this
	static void ApplyAllPassives(Fighter* fighter, std::vector<Passive_ptr>* passive);

	// Make sure to reset the stats before calling this
	static void ApplyStatPassives(Fighter* fighter, std::vector<Passive_ptr> passive);

	// Make sure to reset the skill before calling this
	static void ApplySkillUpgradePassives(Fighter* fighter, std::vector<Passive_ptr> passive);

	//static void ApplySkillPassives(Fighter_ptr fighter, std::vector<Passive_ptr> passive);
};

#endif