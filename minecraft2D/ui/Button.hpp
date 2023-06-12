#pragma once
#include"UiElement.hpp"

#include "ColorButtonAnimatio.hpp"

class Button : public UiElement, public IOnClick, public IOnEnter, public IOnExit, public IOnReleased {
public:
	enum class ButtonState {
		Pressed,
		StandartColor,
		FocusColor
	};

public:
	Button(ButtonAnimation* anim=nullptr,sf::Texture* texture = nullptr, std::string name = "", sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(1, 1), bool active = true) : UiElement(pos, size, name,active) {

		if(texture!=nullptr) 
			m_shape.setTexture(texture);
		if (anim != nullptr) {
			m_animation = anim;
			m_animation->SetAnimateObject(this);
		}
	}


	glm::vec4 getColorAsGlm() { 
		return glm::vec4(m_shape.getFillColor().r, m_shape.getFillColor().g, m_shape.getFillColor().b, m_shape.getFillColor().a);
	}
	sf::Color getColor() {
		return m_shape.getFillColor();
	}
	void setColor(glm::vec4 color) {
		m_shape.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
	}


	void OnEnter(PointEventData data) override {
		m_state = ButtonState::FocusColor;
		m_animation->UpdateState();
	}
	void OnClick(PointEventData data) override {
		m_state = ButtonState::Pressed;
		m_animation->UpdateState();
	}
	void OnReleased(PointEventData data) override {
		m_state = ButtonState::StandartColor;
		m_animation->UpdateState();
	}
	void OnExit(PointEventData data) override {
		m_state = ButtonState::StandartColor;
		m_animation->UpdateState();
	}

	bool LoadFromXmlNode(pugi::xml_node node)  override{
		UiElement::LoadFromXmlNode(node);
		if (node.attribute("Sprite").as_string() != "") {
			m_texture.loadFromFile(node.attribute("Sprite").value());
			m_shape.setTexture(&m_texture);
		}
		return true;
	}

	bool ParseComponent(pugi::xml_node node) override {
		for (auto it = node.begin(); it != node.end(); it++) {
			std::string type = (*it).name();
			if (type == "ColorAnimation") {
				m_animation = ColorButtonAnimation::loadFromXmlNode(*it);
				m_animation->SetAnimateObject(this);
			}
		}
		return true;
	}

	void Update() override {
		UiElement::Update();
		if(m_animation)
			m_animation->ProcessAnim();
	}

	void Draw(sf::RenderTexture* tx) override {
		m_shape.setSize(m_transform.Size());
		m_shape.setPosition(m_transform.GlobalPosition());
		tx->draw(m_shape);
	}

	void HandleGlobalEvent(sf::Event e) override{
		UiElement::HandleGlobalEvent(e);
	}


	void setState(ButtonState state) {
		m_state = state;
	}
	ButtonState getState() {
		return m_state;
	}

protected:
	sf::RectangleShape m_shape;
	sf::Texture m_texture;


	ButtonState m_state;
	ButtonAnimation* m_animation;
};