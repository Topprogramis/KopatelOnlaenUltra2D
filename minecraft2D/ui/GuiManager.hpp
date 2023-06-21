#pragma once
#include"../include.h"

#include "TextBlock.h"
#include "InputBox.hpp"
#include "Group.hpp"
#include "Button.hpp"
#include "Canvas.hpp"
#include "../GuiCommands.h"
#include"../ThreadManagaer.hpp"



class GuiManager {
public:
	GuiManager(sf::Vector2f pos = sf::Vector2f(0,0),sf::Vector2f size = sf::Vector2f(800,600),sf::Color backColor = sf::Color::Black) {
		m_elements = {};
		m_tx = new sf::RenderTexture();
		m_tx->create(size.x, size.y);

		doubleTexture.create(size.x, size.y);
		renderResult.create(size.x, size.y);
		
		m_backColor = backColor;

		m_space.setType(RectTransform::TransformType::Space);

		m_space.LocalPosition(pos);
		m_space.Size() = size;

		m_drawObjects = {};
		m_activeElements = {};
	}

	void setWindow(sf::RenderWindow* window) {
		m_window = window;
	}

	void DrawUiElements() {
		m_tx->clear(m_backColor);

		for (auto element : m_drawObjects) {
			element->Draw(m_tx);
		}

		m_tx->display();

		doubleTexture.update(m_tx->getTexture());

		Settings::threadManager->TryAddCommand("main", new SwapGuiTexture(this, &doubleTexture));
	}
	void Draw() {
		m_shape.setPosition(m_space.GlobalPosition());

		m_shape.setSize(Settings::windowSize);
		m_shape.setTexture(&renderResult);

		m_window->draw(m_shape);
	}
	void SwapTexture(sf::Texture* tx) {
		renderResult.swap(*tx);
	}


	void HandleEvnet(sf::Event e) {
		for (auto& element : m_activeElements) {
			element->HandleGlobalEvent(e);
		}
		if (e.type == sf::Event::MouseButtonPressed) {
			for (auto element : m_elements) {
				UiElement* enterElement;
				if (element->IsEnter(sf::Vector2f(sf::Mouse::getPosition(*m_window)), enterElement)) {
					m_selectElement = enterElement;
				}
			}
		}
		if (m_selectElement!=nullptr) {
			m_selectElement->HandleEvent(e);
		}
	}

	void AddElemnt(UiElement* element) {
		m_elements.push_back(element);
		if (element->getRectTransform().GetSpace() == nullptr) {
			element->SetSpace(&m_space);
		}

		element->OnActiveChange.AddListener(std::bind(&GuiManager::UpdateUiElementsState, this, std::placeholders::_1));

		element->Start();
	}

	sf::RenderTexture* getTexture() {
		return m_tx;
	}

	sf::RectangleShape* getShapePtr() {
		return nullptr;
	}

	void LoadFromFile(std::string path) {
		pugi::xml_document doc;
		if (!doc.load_file(path.c_str())) {
			std::cout << "file not found" << std::endl;
			return;
		}

		pugi::xml_node manager = doc.child("GuiManager");

		for (auto it = manager.begin(); it != manager.end(); it++) {
			AddElemnt(ParseElements(*it,nullptr));
		}

	}

	void GlobalUpdate() {
		for (auto i : m_activeElements) {
			i->GlobalUpdate();
		}
	}
	void Update() {
		m_space.Update();
		for (auto i : m_activeElements) {
			i->Update();
		}
	}
	void FixedUpdate() {
		for (auto i : m_activeElements) {
			i->FixedUpdate();
		}
	}

	UiElement* getElementByName(std::string name) {
		for (auto& i : m_elements) {
			if (i->Name() == name)
				return i;
		}
		return nullptr;
	}

	std::recursive_mutex m_mutex;
	std::condition_variable m_cv;
private:
	sf::RenderWindow* m_window;
	sf::RenderTexture* m_tx;
	sf::RectangleShape m_shape;

	sf::Texture renderResult, doubleTexture;

	sf::Color m_backColor;

	RectTransform m_space;

	UiElement* m_selectElement=nullptr;

	std::vector<UiElement*> m_elements = {};
	std::vector<UiElement*> m_activeElements = {};
	std::vector<UiElement*> m_drawObjects = {};

	UiElement* ParseElements(pugi::xml_node node, UiElement* BaseElement) {
		UiElement* element = nullptr;

		std::string type = node.name();
		if (type == "InputBox") {
			element = new InputBox();
			element->LoadFromXmlNode(node);
		}
		else if (type == "TextBlock") {
			element = new TextBlock();
			element->LoadFromXmlNode(node);
		}
		else if (type == "Group") {
			element = new Group();
			element->LoadFromXmlNode(node);
		}
		else if (type == "Button") {
			element = new Button();
			element->LoadFromXmlNode(node);
		}
		else if (type == "Canvas") {
			element = new Canvas();
			element->LoadFromXmlNode(node);
		}
		else if (type == "Components") {
			if (BaseElement) {
				BaseElement->ParseComponent(node);
				return element;
			}
		}
		else {
			std::cout << "Parse Error" << std::endl;
		}
		
		for (auto it = node.begin(); it != node.end(); it++) {
			UiElement* ChildElement = ParseElements(*it, element);
			
			if (ChildElement) {
				ChildElement->SetSpace(element->getRectTransform().GetSpace());
				ChildElement->SetParent(element);
				AddElemnt(ChildElement);
			}

		}

		return element;
	}


	void UpdateUiElementsState(UiElement* element) {
		auto ActiveIter = std::find(m_activeElements.begin(), m_activeElements.end(), element);
		auto DrawIter = std::find(m_drawObjects.begin(), m_drawObjects.end(), element);


		if (!element->isCostumDraw() && DrawIter != m_drawObjects.end()) {
			m_drawObjects.erase(DrawIter);
			DrawIter = std::find(m_drawObjects.begin(), m_drawObjects.end(), element);
		}

		if (element->GetActive()) {
			if (ActiveIter == m_activeElements.end()) {
				m_activeElements.push_back(element);
			}
			if (DrawIter == m_drawObjects.end() && !element->isCostumDraw()) {
				m_drawObjects.push_back(element);

				for (int i = 0; i < m_drawObjects.size(); i++) {
					for (int j = 0; j < m_drawObjects.size(); j++) {
						if (m_drawObjects[j]->getLayer() > m_drawObjects[i]->getLayer()) {
							auto buf = m_drawObjects[i];
							m_drawObjects[i] = m_drawObjects[j];
							m_drawObjects[j] = buf;
						}
					}
				}
			}
		}
		else {
			if (ActiveIter != m_activeElements.end()) {
				m_activeElements.erase(ActiveIter);
			}
			if (DrawIter != m_drawObjects.end() && !element->isCostumDraw()) {
				m_drawObjects.erase(DrawIter);
			}
		}
	}
};