#include "Transform.h"

Transform::Transform(sf::Vector2f pos, sf::Vector2f size) {
	m_pos = pos;
	m_size = size;
}

void Transform::SetPostion(sf::Vector2f pos) {
	m_pos = pos;
}
void Transform::SetOpenGlPosition(sf::Vector2f pos) {
	m_pos = { pos.x * Settings::windowSize.x,pos.y * Settings::windowSize.y };
}
sf::Vector2f Transform::GetPosition() {
	return m_pos;
}
sf::Vector2f Transform::GetOpenGlPosition() {
	return { m_pos.x / Settings::windowSize.x,m_pos.y / Settings::windowSize.y };
}
void Transform::Move(sf::Vector2f offset) {
	m_pos += offset;
}

void Transform::SetSize(sf::Vector2f size) {
	m_size = size;
}
sf::Vector2f Transform::GetSize() {
	return m_size;
}