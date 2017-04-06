// (c) Ryan Johnson 2017

#include "ContactListener.h"
#include "Box.h"

#include <iostream>




void ContactListener::BeginContact(b2Contact* _contact)
{
	b2Fixture* fixtureA = _contact->GetFixtureA();
	b2Fixture* fixtureB = _contact->GetFixtureB();

	//If one fixture is a sensor and the other a dynamic body then ADD to list of object to calculate buoyancy
	if (fixtureA->IsSensor() && fixtureB->GetBody()->GetType() == b2_dynamicBody)
	{
		//The first fixture will be the water
		buoyPair buoyantPair = make_pair(fixtureA, fixtureB);
		m_buoyancyPairs.insert(buoyantPair);
	}
	else if (fixtureB->IsSensor() && fixtureA->GetBody()->GetType() == b2_dynamicBody)
	{
		buoyPair buoyantPair = make_pair(fixtureB, fixtureA);
		m_buoyancyPairs.insert(buoyantPair);
	}

}

void ContactListener::EndContact(b2Contact* _contact)
{
	b2Fixture* fixtureA = _contact->GetFixtureA();
	b2Fixture* fixtureB = _contact->GetFixtureB();

	//If one fixture is a sensor and the other a dynamic body then REMOVE from list of objects to calculate buoyancy
	if (fixtureA->IsSensor() && fixtureB->GetBody()->GetType() == b2_dynamicBody)
	{
		//The first fixture will be the water
		buoyPair buoyantPair = make_pair(fixtureA, fixtureB);
		m_buoyancyPairs.erase(buoyantPair);
	}
	else if (fixtureB->IsSensor() && fixtureA->GetBody()->GetType() == b2_dynamicBody)
	{
		buoyPair buoyantPair = make_pair(fixtureB, fixtureA);
		m_buoyancyPairs.erase(buoyantPair);
	}

}