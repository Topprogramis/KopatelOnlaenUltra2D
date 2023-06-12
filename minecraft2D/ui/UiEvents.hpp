#pragma once
#include"../include.h"

struct PointEventData {
	sf::Vector2f pos;
	sf::Event::EventType type;
	UiElement* object;

	sf::Mouse::Button bt;
};


class UiEvent {

};

//Mouse button event
class IOnDown : public UiEvent {
public:
	virtual void OnDown(PointEventData data) abstract;
};

class IOnClick : public UiEvent {
public:
	virtual void OnClick(PointEventData data) abstract;
};

class IOnReleased : public UiEvent {
public:
	virtual void OnReleased(PointEventData data) abstract;
};

//Drag event
class IOnStartDrag : public UiEvent {
public:
	virtual void OnStartDrag(PointEventData data) abstract;
};

class IOnDrag : public UiEvent {
public:
	virtual void OnDrag(PointEventData data) abstract;
};

class IOnEndDrag : public UiEvent {
public:
	virtual void OnEndDrag(PointEventData data) abstract;
};

//Mouse move event

class IOnEnter : public UiEvent {
public:
	virtual void OnEnter(PointEventData data) abstract;
};

class IOnExit : public UiEvent {
public:
	virtual void OnExit(PointEventData data) abstract;
};

class IOnFocus : public UiEvent {
public:
	virtual void OnFocus(PointEventData data) abstract;
};