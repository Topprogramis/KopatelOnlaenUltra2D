#pragma once
#include"UiElement.hpp"

class Canvas : public UiElement {
public:
	Canvas(sf::Color backColor = sf::Color(0,0,0), std::string name = "", sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(100, 100), bool active = true) : UiElement(pos, size, name, active) {
		m_renderSpace = new sf::RenderTexture();
		m_renderSpace->create(m_transform.Size().x, m_transform.Size().y);

		OnAddChild.AddListener(std::bind(&Canvas::InitNewObject, this, std::placeholders::_1));

		//m_transform.setType(RectTransform::TransformType::Space);

		m_transform.LocalPosition(pos);
		m_transform.Update();
	}

	//xml load

	bool LoadFromXmlNode(pugi::xml_node node)  override {
		UiElement::LoadFromXmlNode(node);
		if (node.attribute("BackgroundColor").as_string() != "") {
			m_backColor = ColorFromString(node.attribute("BackgroundColor").value());
		}
		return true;
	}

	//transform update

	void OnTransformEdit() {
		m_transform.UpdateFlag();

		m_shape.setSize(m_transform.Size());
		m_renderSpace->create(m_transform.Size().x, m_transform.Size().y);
	}

	//draw

	void Draw(sf::RenderTexture* tx) override {
#ifdef DRAW_GIZMO
		DrawGizmo(tx);
#endif
		m_renderSpace->clear(m_backColor);
		OnDrawObjects();
		m_renderSpace->display();
		m_renderResult = m_renderSpace->getTexture();

		m_shape.setPosition(m_transform.GlobalPosition());
		m_shape.setTexture(&m_renderSpace->getTexture());

		tx->draw(m_shape);
	}

	//create new object

	void InitNewObject(UiElement* element) {
		element->setCostumDraw(true);
		element->getRectTransform().SetParent(&m_object);
		element->getRectTransform().SetSpace(&m_transform);
		m_elements.push_back(element);
		
	}

	void Start() override {
		UiElement::Start();
	}

	//game loop

	void Update() override{
		m_transform.Update();																																														m_transform.Update();
	}

	

protected:
	sf::Color m_backColor;
	sf::RectangleShape m_shape;

	RectTransform m_object;

	sf::Texture m_renderResult;

	sf::RenderTexture* m_renderSpace;

	std::vector<UiElement*> m_elements = {};


	virtual void OnDrawObjects() {
		for (auto& i : m_elements) {
			i->Draw(m_renderSpace);
		}
	}
};