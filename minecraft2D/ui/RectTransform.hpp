#pragma once
#include"../include.h"
#include"../Action.h"

class RectTransform {
public:
	enum class TransformType {
		Space,
		Object,
		ObjectSpace
	};
public:
	RectTransform(sf::Vector2f pos = sf::Vector2f(0,0), sf::Vector2f size = sf::Vector2f(0, 0), float rot=0.f, TransformType type = TransformType::Object) {
		m_localPosition = pos;
		m_size = size;
		m_rot = rot;

		m_type = type;

		UpdateFlag();
	}

	sf::Vector2f LocalPosition() { 
		return m_localPosition; 
	}
	void LocalPosition(sf::Vector2f pos) {
		m_localPosition = pos;

		UpdateFlag();
		
		for (auto& i : m_childrens) {
			i->UpdateFlag();
		}
	}
	sf::Vector2f ScreenPosition() {
		return m_screenPosition;
	}
	sf::Vector2f GlobalPosition() {
		return m_globalPosition;
	}

	void Size(sf::Vector2f vec) { m_size = vec; }
	sf::Vector2f& Size() { return m_size; }
	float& Rotation() { return m_rot; }

	void setType(TransformType type) {
		m_type = type;

		m_updateFlag = true;
	}
	TransformType Type() {
		return m_type;
	}

	void SetSpace(RectTransform* space) {
		UpdateFlag();
		m_space = space;
	}
	RectTransform* GetSpace() {
		return m_space;
	}
	void SetParent(RectTransform* parent) {
		UpdateFlag();
		m_parent = parent;
		if(parent)
			parent->AddChild(this);
	}
	RectTransform* GetParent() {
		return m_parent;
	}
	void AddChild(RectTransform* child) {
		if (child->GetParent() != this) {
			child->SetParent(this);
		}
		m_childrens.push_back(child);
	}

	void UpdateFlag() {
		m_updateFlag = true;
	}

	void Update() {
		if (m_updateFlag) {
			CalculateGlobalPosition();
			CalculateScreenPosition();
			for (auto& i : m_childrens) {
				i->UpdateFlag();
			}
			m_updateFlag = false;
		}
	}

	Action<RectTransform*> OnTransformEdit;

private:
	sf::Vector2f m_localPosition, m_size, m_globalPosition, m_screenPosition;
	float m_rot;

	TransformType m_type;

	bool m_updateFlag;

	RectTransform* m_space = nullptr;
	RectTransform* m_parent = nullptr;

	std::vector<RectTransform*> m_childrens = {};

	void CalculateGlobalPosition() {
		if (m_parent != nullptr) {
			m_globalPosition = m_localPosition + m_parent->GlobalPosition();
		}
		else {
			m_globalPosition = m_localPosition;
		}
	}

	void CalculateScreenPosition() {
		if (m_space != nullptr) {
			m_screenPosition = GlobalPosition() + m_space->ScreenPosition();
		}
		else {
			m_screenPosition = GlobalPosition();
		}
	}

};