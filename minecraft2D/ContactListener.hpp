#include"include.h"
#include"CollisionEvents.hpp"

class ContactListener : public b2ContactListener {
public:
    void EndContact(b2Contact* contact) override {
        //getData
        CollisionData dataA = *static_cast<CollisionData*>(contact->GetFixtureA()->GetBody()->GetUserData());       
        CollisionData dataB = *static_cast<CollisionData*>(contact->GetFixtureB()->GetBody()->GetUserData());

        //set data
        dataA.contact = contact;
        dataB.contact = contact;

        dataA.collisionBId = dataB.collisionId;
        dataA.objectBType = dataB.objectBType;
        dataA.objectB = dataB.objectB;

        dataB.collisionBId = dataA.collisionId;
        dataB.objectBType = dataA.objectBType;
        dataB.objectB = dataA.objectB;


        if (dataA.exitE)
            static_cast<ICollisionExitEvent*>(dataA.exitE)->OnCollisionExit(dataA);

        if (dataB.exitE)
            static_cast<ICollisionExitEvent*>(dataB.exitE)->OnCollisionExit(dataB);
    }

    void BeginContact(b2Contact* contact) override {
        //getData
        CollisionData dataA = *static_cast<CollisionData*>(contact->GetFixtureA()->GetBody()->GetUserData());
        CollisionData dataB = *static_cast<CollisionData*>(contact->GetFixtureB()->GetBody()->GetUserData());

        //set data
        dataA.contact = contact;
        dataB.contact = contact;

        dataA.collisionBId = dataB.collisionId;
        dataA.objectBType = dataB.objectBType;
        dataA.objectB = dataB.objectB;

        dataB.collisionBId = dataA.collisionId;
        dataB.objectBType = dataA.objectBType;
        dataB.objectB = dataA.objectB;


        if (dataA.enterE)
            static_cast<ICollisionEnterEvent*>(dataA.enterE)->OnCollisionEnter(dataA);
        if (dataB.enterE)
            static_cast<ICollisionEnterEvent*>(dataB.enterE)->OnCollisionEnter(dataB);
    }

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {

    }

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {

    }
};
