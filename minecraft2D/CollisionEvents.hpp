#pragma once
#include"include.h"

__interface ICollisionEvent {

};

struct CollisionData {
	int collisionId;
	std::string objectType;
	void* object;

	//collision data
	b2Contact* contact = nullptr;
	b2Manifold* oldManifold = nullptr;

	int collisionBId;
	std::string objectBType;
	void* objectB;

	//events
	ICollisionEvent* enterE = nullptr;
	ICollisionEvent* exitE = nullptr;
	ICollisionEvent* preSolveE = nullptr;
	ICollisionEvent* solveE = nullptr;
};

__interface ICollisionEnterEvent : public ICollisionEvent {
public:
	virtual void OnCollisionEnter(CollisionData data) = 0;
};
class ICollisionExitEvent : public ICollisionEvent {
public:
	virtual void OnCollisionExit(CollisionData data) = 0;
};
class ICollisionPreSolveEvent : public ICollisionEvent {
	virtual void OnCollisionPreSolve(CollisionData data) = 0;
};
class ICollisionSolveEvent : public ICollisionEvent {
	virtual void OnCollisionSolve(CollisionData data) = 0;
};
