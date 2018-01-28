#include "battleManager.h"
#include "input_manager.h"
#include <GLFW\glfw3.h>
#include <set>
#include <algorithm>
#include <iostream>
#include "fontManager.h"
#include "gameData.h"

BattleManager::BattleManager()
{
	Init();
}

BattleManager::BattleManager(std::vector<Actor_ptr> actors)
{
	// Populate actors
	for (auto a : actors)
		_actors.push_back(a);

	std::sort(actors.begin(), actors.end(), Actor::ActorSpeedSort);
	for (auto a : actors)
		_actorQueue.push_back(a);

	Init();
}

void BattleManager::Init()
{
	_hud.Init(_actors);
	_showingSkills = false;
	_state = BS_SelectAction;
	m_animating = false;
	counter = 0;
	_winner = -1;
	_selectedIndex = 0;
	_done = false;
	if (_actorQueue.size() > 0)
	{
		_chooseSkill = &_actorQueue.front()->Skills;
		_owner = _actorQueue.front();
	}
}

void BattleManager::Update()
{
	// Return when the battle is over
	if (_done)
		return;

	// Cout battle state
	//counter++;
	//if (counter % 120 == 0)
	//	std::cout << "State: " << _state << std::endl;

	_winner = FindWinner();
	if (_winner == -1 || _selectedSkill && !_selectedSkill->_done)
	{
		// Battle stuffs
		ManageInput();
		UpdateLogic();
		if (_animations.size() > 0)
		{
			_animations.front()->Update();
			if (_animations.front()->_done)
			{
				_animations.pop_front();
				//if (_animations.size() == 0)
				//	_state = BS_ActionProgress;
			}
		}

		_hud.Update();
	}
	else
	{
		// Battle is over, theres a winner etc
		std::cout << "The winner is: team " << _winner << std::endl;
		_done = true;
	}
}

void BattleManager::UpdateLogic()
{
	if (_state == BS_ActionDone)
	{
		_selectedSkill->Reset();
		_selectedSkill = NULL;
		_state = BS_SelectAction;
		CycleActors();

		if (_owner->Team == 0)
		{
			SetChooseSkillText();
			_showingSkills = true;
		}

		Select(0);
	}

	if (_showingSkills)
	{
		if (_state != BS_SelectAction)
		{
			RemoveChooseSkillText();
			_showingSkills = false;
		}
	}

	// Let the animation pass before updating
	//if (_animations.size() < 1)
	//{
		// End turn if the skill is done
	if (_selectedSkill == NULL)
	{
		_owner = _actorQueue.front();

		// If the actor is dead, cycle
		if (_owner->Dead || _state == BS_ActionDone)
		{
			CycleActors();
			Select(0);
			return;
		}

		// Choose enemy action
		if (_owner->Team != 0)
		{
			_selectedSkill = _owner->Skills.at(rand() % _owner->Skills.size());

			int targ = 0;
			do {
				targ = rand() % _actors.size();
			} while (targ < 0 || targ > _actors.size() || _actors.at(targ)->Team == _owner->Team);

			_targets.push_back(_actors.at(targ));

			UseSkill();
		}
	}
	else
	{
		// Update skill
		if (_state != BS_SelectTargets)
		{
			if (_selectedSkill != NULL)
			{
				if (_selectedSkill->_done)
				{
					_state = BS_ActionDone;
				}
				else
				{
					_selectedSkill->Update();
				}
			}
		}
	}
	//}
}

void BattleManager::SetChooseSkillText()
{
	while (_fonts.size() < _chooseSkill->size())
		_fonts.push_back(FontManager::GetInstance().AddFont(false, false, true, "res/fonts/lowercase.png"));

	for (int i = 0; i < _chooseSkill->size(); i++)
	{
		FontManager::GetInstance().EnableFont(_fonts[i]);
		FontManager::GetInstance().SetScale(_fonts[i], 0.5f, 0.5f);
		FontManager::GetInstance().SetText(_fonts[i], _(_chooseSkill->at(i)->_name), Vector3f(7.5f, 5.5f - i * 0.5f, 0));
		//FontManager::GetInstance().GetFont(_fonts[i])->SetText(_chooseSkill->at(i)->_name, Vector3f(6, 7 - i, 1));
	}
}

void BattleManager::RemoveChooseSkillText()
{
	for (int i = 0; i < _fonts.size(); i++)
		FontManager::GetInstance().GetFont(_fonts[i])->_enabled = false;
}


void BattleManager::ManageInput()
{
	// Get input
	std::set<int> input;
	if (InputManager::GetInstance().FrameKeyStatus(GLFW_KEY_SPACE, KeyStatus::Release, 5))
		input.emplace(GLFW_KEY_SPACE);
	if (InputManager::GetInstance().FrameKeyStatus(GLFW_KEY_C, KeyStatus::Release, 5))
		input.emplace(GLFW_KEY_C);
	for (int i = GLFW_KEY_RIGHT; i < GLFW_KEY_UP + 1; i++)
		if (InputManager::GetInstance().FrameKeyStatus(i, KeyStatus::KeyPressed, 5))
			input.emplace(i);

	// If theres an animation going, SEND THE INPUT TO THE SKILL
	if (_state != BS_SelectTargets && _state != BS_SelectAction)
	{
		if (_selectedSkill != NULL)
			_selectedSkill->_input = input;
		return;
	}

	// Handle Input
	if (input.count(GLFW_KEY_DOWN))
	{
		// Attempt to manage DOWN logic immediately here
		if (_state == BS_SelectAction)
		{
			if (_selectedIndex < _chooseSkill->size() - 1)
				Select(_selectedIndex + 1);
		}
		else if (_state == BS_SelectTargets)
		{
			if (_selectedIndex > 0)
			{
				for (int i = _selectedIndex - 1; i >= 0; i--)
				{
					if (_actors[i]->RespectsTargeting(_owner, _selectedSkill->_targetMode))
					{
						Select(i);
						break;
					}
				}
			}
			else
			{
				Select(_selectedIndex);
			}
		}
	}

	if (input.count(GLFW_KEY_UP))
	{
		// Attempt to manage UP logic immediately here
		if (_state == BS_SelectAction)
		{
			if (_selectedIndex > 0)
			{
				Select(_selectedIndex - 1);
			}
		}
		else if (_state == BS_SelectTargets)
		{
			if (_selectedIndex < _actors.size() - 1)
			{
				for (int i = _selectedIndex + 1; i < _actors.size(); i++)
				{
					if (_actors[i]->RespectsTargeting(_owner, _selectedSkill->_targetMode))
					{
						Select(i);
						break;
					}
				}
			}
		}
	}

	if (input.count(GLFW_KEY_LEFT))
	{
		// Attempt to manage LEFT logic immediately here
	}

	if (input.count(GLFW_KEY_RIGHT))
	{
		// Attempt to manage RIGHT logic immediately here
	}

	if (input.count(GLFW_KEY_SPACE))
	{
		if (_state == BS_SelectAction)
		{
			_selectedSkill = _chooseSkill->at(_selectedIndex);
			//std::cout << "Skill selected: " << _selectedSkill->_name << std::endl;
			_state = BS_SelectTargets;
			bool done = false;
			for (int i = 0; i < _actors.size(); i++)
			{
				switch (_selectedSkill->_defaultTarget)
				{
				case DT_Self:
					if (_actors[i] == _owner)
					{
						Select(i);
						done = true;
					}
					break;
				case DT_Ally:
					if (_actors[i]->Team == _owner->Team && _actors[i]->RespectsTargeting(_owner, _selectedSkill->_targetMode))
					{
						Select(i);
						done = true;
					}
					break;
				case DT_Enemy:
					if (_actors[i]->Team != _owner->Team && _actors[i]->RespectsTargeting(_owner, _selectedSkill->_targetMode))
					{
						Select(i);
						done = true;
					}
					break;
				}

				if (done)
					break;
			}
		}
		else if (_state == BS_SelectTargets)
		{
			for (auto x : _actorQueue)
				if (x->Selected)
				{
					_targets.push_back(x);
					x->SetColorAll();
					x->Selected = false;
				}

			if (_targets.size() < _selectedSkill->_minTargets)
				return;

			_selectedSkill->Reset();
			UseSkill();
		}
	}

	if (input.count(GLFW_KEY_C))
	{
		if (_state == BS_SelectTargets)
		{
			_selectedIndex = 0;
			_state = BS_SelectAction;
			SetChooseSkillText();
			Select(_selectedIndex);
			_showingSkills = true;
			for (auto x : _actors)
			{
				x->ApplyLethal();
				x->Selected = false;
			}
		}
	}
}

void BattleManager::Select(int target)
{
	if (_state == BS_SelectTargets)
	{
		for (auto x : _actors)
		{
			if (x->Selected)
				x->ApplyLethal();
			x->Selected = false;
		}

		_actors[_selectedIndex]->Selected = false;
		_actors.at(target)->Selected = true;
		_actors.at(target)->SetColorAll(Vector3f(1.f, 0.25f, 0.25f));
		_selectedIndex = target;
	}
	else if (_state == BS_SelectAction)
	{
		if (_owner->Team == 0)
		{
			FontManager::GetInstance().GetFont(_fonts[_selectedIndex])->GetGraphics()->SetColorAll();
			FontManager::GetInstance().GetFont(_fonts[target])->GetGraphics()->SetColorAll(Vector3f(1, 0, 0));
		}
		_selectedIndex = target;
	}
}

void BattleManager::UseSkill()
{
	//std::cout << "Using skill: " << _selectedSkill->_name << std::endl;
	if (_selectedSkill != NULL)
		_state = _selectedSkill->Start(&_targets, &_actorQueue, &_animations, _owner);
	else
		_state = BS_SelectAction;
}

void BattleManager::CycleActors()
{
	Actor_ptr front = _actorQueue.front();
	_actorQueue.pop_front();
	_actorQueue.push_back(front);
	_actorQueue.front()->ChoosingAction = true;
	_chooseSkill = &_actorQueue.front()->Skills;
	_targets.clear();
	_selectedIndex = 0;
	_owner = _actorQueue.front();
	if (!_actorQueue.front()->Dead)
		return;
	//std::cout << "It's " << _actorQueue.front()->Name << "'s turn!" << std::endl;
	else
		CycleActors();
}

// Return winning team, -1 if battle isn't over
int BattleManager::FindWinner()
{
	std::set<int> activeTeams;
	for (auto x : _actorQueue)
	{
		// Dont bother continuing if the fight isn't over
		if (activeTeams.size() > 1)
			break;
		// If someone isn't dead on that team, that team is still going
		if (!x->Dead && !activeTeams.count(x->Team))
			activeTeams.emplace(x->Team);
	}

	// More than one team is alive, return -1
	if (activeTeams.size() != 1)
		return -1;

	// Only one team is alive, return that team
	return *activeTeams.begin();
}

void BattleManager::SetRender()
{
	_hud.SetRender();
}
