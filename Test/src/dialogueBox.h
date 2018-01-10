#ifndef DIALOGUE_BOX_H__
#define DIALOGUE_BOX_H__

#include "font.h"
#include "input_manager.h"
#include "iEvent.h"
#include "dialogueGraph.h"
#include "eventQueue.h"

class DialogueBox : public Font, public IEvent
{
public:
	DialogueBox(unsigned int entity_id, std::vector<Dialogue> d, std::vector<DialogueChoice> dc);
	DialogueBox(unsigned int entity_id, std::shared_ptr<DialogueGraph> dg);
	~DialogueBox();
	void Construct();
	void Update(double elapsedTime);
	void UpdateBox();
	void ResetEvent();
	EventUpdateResponse UpdateEvent(double elapsedTime, std::map<unsigned int, std::shared_ptr<Entity>>* ents);
	std::shared_ptr<IEvent> Clone();
	void Draw();
	void SetRender();
	void SetText(std::string text);
	void SetScale(float xScale, float yScale);

private:
	std::shared_ptr<FontGraphicsComponent> m_box;
	std::vector<Font_ptr> m_choices;
	float m_maxWidth;
	float m_maxHeight;
	bool m_firstTime;
	std::shared_ptr<DialogueGraph> m_dialogueGraph;
	static DialogueBox* m_owner;
};

#endif // !DIALOGUE_BOX_H__
