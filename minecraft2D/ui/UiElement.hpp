#pragma once
#include "RectTransform.hpp"
#include "Convertor.h"
#include "UiEvents.hpp"
#include "../Action.h"
#include"Animator.hpp"

__interface IDrawable {
	void Draw(sf::RenderTexture* tx);
};

class UiElement : public IDrawable, public IAnimatable {
public:
	UiElement(sf::Vector2f pos, sf::Vector2f size, std::string name, bool active) : m_transform(pos, size) {
		Name() = name;

#ifdef DRAW_GIZMO
		m_gizmo = sf::RectangleShape(size);
		m_gizmo.setFillColor(sf::Color(0, 0, 0, 0));
		m_gizmo.setOutlineColor(GIZMO_COLOR);
		m_gizmo.setOutlineThickness(GIZMO_OUTLINE_SIZE);
#endif //DRAW GIZMO

		m_active = active;
	}


	bool UpdateAnimateParm(std::string name, glm::vec4 value) override{
		if (name == "position")
			m_transform.LocalPosition(sf::Vector2f(value.x, value.y));
		if (name == "size")
			m_transform.Size(sf::Vector2f(value.x, value.y));
		return true;
	}



	//load from xml
	virtual bool ParseComponent(pugi::xml_node node) {
		return true;
	}

	virtual bool LoadFromXmlNode(pugi::xml_node node) {
		if (node.attribute("Pos").as_string() != "") {
			sf::Vector2f pos = Vec2FromString(node.attribute("Pos").as_string());
			getRectTransform().LocalPosition(pos);
		}
		if (node.attribute("Size").as_string() != "") {
			sf::Vector2f size = Vec2FromString(node.attribute("Size").as_string());
			getRectTransform().Size() = size;
			m_gizmo.setSize(size);
		}
		if (node.attribute("Name").as_string() != "") {
			m_name = node.attribute("Name").as_string();
		}
		if (node.attribute("Active").as_string() != "") {
			SetActive(node.attribute("Active").as_bool());
		}
		return true;
	}

	//events

	bool IsEnter(sf::Vector2f pos, UiElement*& element) {
		if (CheakCollide(pos)) {
			element = this;
			return true;
		}
		else {
			return false;
		}
	}

	virtual void HandleEvent(sf::Event e) {

	}
	virtual void HandleGlobalEvent(sf::Event e) {
		if (e.type == sf::Event::MouseMoved) {
			PointEventData data;
			data.object = this;
			data.pos = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
			data.type = e.type;

			if (m_OnDown) {
				if (m_OnDrag) {
					if (dynamic_cast<IOnDrag*>(this)) {
						dynamic_cast<IOnDrag*>(this)->OnDrag(data);
					}
				}
				else {
					m_OnDrag = true;
					if (dynamic_cast<IOnStartDrag*>(this)) {
						dynamic_cast<IOnStartDrag*>(this)->OnStartDrag(data);
					}
				}
			}

			if (CheakCollide(sf::Vector2f(e.mouseMove.x, e.mouseMove.y))) {

				if (m_OnEnter) {
					if (dynamic_cast<IOnFocus*>(this)) {
						dynamic_cast<IOnFocus*>(this)->OnFocus(data);
					}
				}
				else {
					m_OnEnter = true;

					if (dynamic_cast<IOnEnter*>(this)) {
						dynamic_cast<IOnEnter*>(this)->OnEnter(data);
					}
				}
			}
			else {
				if (m_OnEnter) {
					m_OnEnter = false;
					if (dynamic_cast<IOnExit*>(this)) {
						dynamic_cast<IOnExit*>(this)->OnExit(data);
					}
				}
			}
		}
		else if (e.type == sf::Event::MouseButtonPressed) {
			PointEventData data;

			data.object = this;
			data.pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			data.type = e.type;
			data.bt = e.mouseButton.button;

			if (CheakCollide(data.pos)) {

				if (m_OnDown) {
					if (dynamic_cast<IOnDown*>(this)) {
						dynamic_cast<IOnDown*>(this)->OnDown(data);
					}
				}
				else {
					m_OnDown = true;
					if (dynamic_cast<IOnClick*>(this)) {
						dynamic_cast<IOnClick*>(this)->OnClick(data);
					}
				}
			}
		}
		else if (e.type == sf::Event::MouseButtonReleased) {
			PointEventData data;

			data.object = this;
			data.pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
			data.type = e.type;
			data.bt = e.mouseButton.button;

			if (m_OnDrag) {
				m_OnDrag = false;
				if (dynamic_cast<IOnEndDrag*>(this)) {
					dynamic_cast<IOnEndDrag*>(this)->OnEndDrag(data);
				}
			}

			if (m_OnDown) {

				if (CheakCollide(data.pos)) {
					if (dynamic_cast<IOnReleased*>(this)) {
						dynamic_cast<IOnReleased*>(this)->OnReleased(data);
					}
				}
				m_OnDown = false;
			}
		}
	}

	//game loop

	virtual void Start() {
		OnActiveChange.Invoke(this);
	}

	virtual void Update() {
		m_transform.Update();
	}
	virtual void GlobalUpdate() {

	}
	virtual void OnTransformEdit() {

	}

	virtual void FixedUpdate() {

	}

	//draw
	void Draw(sf::RenderTexture* tx) override {
#ifdef DRAW_GIZMO
		DrawGizmo(tx);
#endif
	}

	void DrawGizmo(sf::RenderTexture* tx) {
		m_gizmo.setPosition(m_transform.GlobalPosition());
		tx->draw(m_gizmo);
	}

	//proerties
	

	void SetSpace(RectTransform* space) {
		getRectTransform().SetSpace(space);
	}
	void SetParent(UiElement* parent) {
		m_parent = parent;
		if (parent != nullptr) {
			getRectTransform().SetParent(&parent->getRectTransform());
			parent->AddChildren(this);
			parent->OnActiveChange.AddListener(std::bind(&UiElement::OnParentActiveChange, this, std::placeholders::_1));
		}
		else {
			getRectTransform().SetParent(nullptr);
		}
	}
	UiElement* GetParent() {
		return m_parent;
	}
	void AddChildren(UiElement* element) {
		m_childrens.push_back(element);
		OnAddChild.Invoke(element);
		if (element->GetParent() != this) {
			element->SetParent(this);
		}
	}
	RectTransform& getRectTransform() {
		OnTransformEdit();
		return m_transform;
	}

	void setLayer(int layer) {
		m_layer = layer;
	}
	int getLayer() {
		return m_layer;
	}

	std::string& Name() { return m_name; }

	void SetActive(bool state) {
		m_active = state;
		OnActiveChange.Invoke(this);
	}
	bool GetActive() {
		return m_active;
	}
	void setCostumDraw(bool state) {
		m_customDraw = state;
		OnActiveChange.Invoke(this);
	}
	bool isCostumDraw() {
		return m_customDraw;
	}
	Action<UiElement*> OnActiveChange;
	Action<UiElement*> OnAddChild;

protected:
	RectTransform m_transform;
	int m_layer = 0;

	bool m_active, m_customDraw;

	sf::RenderWindow* window;

	sf::RectangleShape m_gizmo;
	std::string m_name;

	std::vector<UiElement*> m_childrens;
	UiElement* m_parent;

	bool m_OnEnter, m_OnDown, m_OnDrag;


	bool CheakCollide(sf::Vector2f pos) {
		return pos.x >= m_transform.ScreenPosition().x && pos.x <= m_transform.ScreenPosition().x + m_transform.Size().x && pos.y >= m_transform.ScreenPosition().y && pos.y  <= m_transform.ScreenPosition().y + m_transform.Size().y;
	}

	void OnParentActiveChange(UiElement* element) {
		if (element->GetActive()==false) {
			SetActive(false);
		}
	}




};