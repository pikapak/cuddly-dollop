#ifndef EVENT_MAP_CHANGE_H__
#define EVENT_MAP_CHANGE_H__

#include "iEvent.h"
#include "entity.h"

class EventMapChange : public IEvent
{
public:
	EventMapChange(unsigned int map);
	~EventMapChange() {}
	EventUpdateResponse UpdateEvent(double elapsedTime, std::map<unsigned int, std::shared_ptr<Entity>>* ents);
	void ResetEvent();

private:
	unsigned int m_map;
};

#endif // !DIALOGUE_BOX_H__
