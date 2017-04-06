// (c) Ryan Johnson 2017

#ifndef _CONTACT_LISTENER_H
#define _CONTACT_LISTENER_H

#include <Box2D/Box2D.h>
#include <string>
#include <set>

struct ContactID
{
	std::string objectType;
	void* object;
};

typedef std::pair<b2Fixture*, b2Fixture*> buoyPair;

class ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* _contact);
	void EndContact(b2Contact* _contact);

	std::set<buoyPair>* getBuyoancyPairs() { return &m_buoyancyPairs; }

private:
	std::set<buoyPair> m_buoyancyPairs;

};

#endif