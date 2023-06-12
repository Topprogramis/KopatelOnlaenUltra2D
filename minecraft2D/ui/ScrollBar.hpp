#pragma once
#include"ui/Button.hpp"
#include"RectRender.hpp"

class BarHandle : public Button, public IOnDrag {
public:
	BarHandle(ButtonAnimation* anim = nullptr, sf::Texture* texture = nullptr, sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(1, 1)) : Button(anim, texture, "handle", pos, size, true) {

	}

	void SetSplitSize(float value) {
		m_splitSize = value;
	}
	void SetBarSize(sf::Vector2f value) {
		m_barSize = value;
	}

	float getValue() {
		return m_offset.x;
	}

	void OnDrag(PointEventData data) override {
		auto newOffset = (data.pos-(m_transform.Size()*0.5f)) - m_transform.GetParent()->ScreenPosition();

		m_offset.x = std::max(0.f, std::min(newOffset.x, (m_transform.GetParent()->Size().x - m_transform.Size().x)));
		m_offset.y=(m_transform.GetParent()->Size().y - m_transform.Size().y) / 2;

		m_transform.LocalPosition(m_offset);

		OnHandleMove.Invoke();
	}

	Action<> OnHandleMove;
private:
	sf::Vector2f m_offset, m_barSize;

	float m_splitSize;
};

class ScrollBar : public UiElement {
public:
	ScrollBar(sf::Texture* backTexture = nullptr, sf::Texture* filledBackTexture = nullptr, BarHandle* handle = nullptr, float min = 0, float max = 1,sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(1, 1), std::string name = "",bool active = true) : UiElement(pos, size, name, active) {
		m_backTexture = backTexture;

		m_handle = handle;
		m_handle->SetParent(this);

		m_handle->OnHandleMove.AddListener(std::bind(&ScrollBar::OnHandleMove, this));

		m_filledBackTexture = filledBackTexture;

		m_fill.setTexture(m_filledBackTexture);


		m_back = sf::RectangleShape(size);

		m_back.setTexture(m_backTexture);

		SetBoundsSize(min, max);
		m_handle->SetSplitSize(m_splitSize);
	}

	void SetBoundsSize(float min, float max) {
		m_splitSize = ((m_transform.Size().x-m_handle->getRectTransform().Size().x) / (max - min));

		m_min = min;
		m_max = max;

		m_handle->SetSplitSize(m_splitSize);
	}
	void SetBackgroundTexture(sf::Texture* tx) {
		m_backTexture = tx;
		m_back.setTexture(tx);
	}

	float getValue() {
		return m_min+(m_handle->getValue() / m_splitSize);
	}

	void Draw(sf::RenderTexture* tx) override {
		m_back.setPosition(m_transform.GlobalPosition());
		m_fill.setPosition(m_transform.GlobalPosition());

		tx->draw(m_back);
		tx->draw(m_fill);

		m_handle->Draw(tx);
	}
	void Update() override {
		UiElement::Update();

		m_handle->Update();
	}

	void HandleGlobalEvent(sf::Event e) override {
		UiElement::HandleGlobalEvent(e);
		m_handle->HandleGlobalEvent(e);
	}

	Action<float> OnValueChange;
private:
	sf::Texture* m_backTexture, *m_filledBackTexture;
	sf::RectangleShape m_back, m_fill;

	BarHandle* m_handle;

	float m_splitSize, m_min, m_max;

	void OnHandleMove() {
		m_fill.setSize(sf::Vector2f(m_handle->getValue() + (m_handle->getRectTransform().Size().x / 2), m_transform.Size().y));
		m_fill.setTextureRect(sf::IntRect(0, 0, m_fill.getSize().x, m_fill.getSize().y));

		OnValueChange.Invoke(getValue());
	}
};