#include "inputComponent.h"

InputComponent::InputComponent() : m_pos(Vector3f()), m_eventQueue(EventQueue())
{

}

void InputComponent::Update()
{
	m_eventQueue.Update(ElapsedTime::GetInstance().GetElapsedTime());
}


std::vector<std::string> InputComponent::Interact()
{
	DialogueBox* db = new DialogueBox();
	db->SetScale(0.5f, 0.5f);
	db->SetTextSpeed(0.03f);
	db->SetText("Hello!");

	DialogueBox* db2 = new DialogueBox();
	db2->SetScale(0.5f, 0.5f);
	db2->SetTextSpeed(0.02f);
	db2->SetText("I'm talking to you right now!");

	m_eventQueue.PushBack(db);
	m_eventQueue.PushBack(db2);

	//unsigned int font = FontManager::GetInstance().AddDialogueBox();
	//Vector3f pos = m_pos;
	//pos.y += 1.0f;
	//pos.z = 0;
	//pos.x += 0.5f;
	//FontManager::GetInstance().SetScale(font, 0.5f, 0.5f);
	//FontManager::GetInstance().SetTextSpeed(font, 0.03f);
	//FontManager::GetInstance().SetText(font, "Hello!", pos, true);

	return std::vector<std::string>()/* = { "TELEPORT", "6", "6", "4" }*/; 
}

void InputComponent::ReceiveMessage(std::vector<std::string> msg)
{
	if (msg.size() == 0)
		return;
	if (msg.at(0) == "SET_POSITION")
	{
		if (msg.size() >= 4)
		{
			m_pos = Vector3f((float)::atof(msg.at(1).c_str()), (float)::atof(msg.at(2).c_str()), (float)::atof(msg.at(3).c_str()));
		}
	}
}
