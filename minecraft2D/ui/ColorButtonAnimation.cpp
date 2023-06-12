#include"ColorButtonAnimatio.hpp"
#include"Button.hpp"
#include"../TimeManage.h"

void ColorButtonAnimation::ProcessAnim()
{
	switch (m_state) {
	case AnimationState::End:
		UpdateNeedColor();
		m_step = (m_needColor - m_color) / m_time;
		m_state = AnimationState::Play;
		break;
	case AnimationState::Play:
		m_color = glm::clamp(m_color+m_step,glm::vec4(0), glm::vec4(255));
		if (m_color == m_needColor) {
			m_state = AnimationState::Stop;
		}

		m_animateButton->setColor(m_color);
		break;
	case AnimationState::Stop:
		return;
		break;
	}



}

void ColorButtonAnimation::UpdateNeedColor() {
	switch (m_animateButton->getState()) {
	case Button::ButtonState::Pressed:
		m_needColor = glm::vec4(m_pressedColor.r, m_pressedColor.g, m_pressedColor.b, m_pressedColor.a);
		break;
	case  Button::ButtonState::StandartColor:
		m_needColor = glm::vec4(m_standartColor.r, m_standartColor.g, m_standartColor.b, m_standartColor.a);
		break;
	case  Button::ButtonState::FocusColor:
		m_needColor = glm::vec4(m_focusColor.r, m_focusColor.g, m_focusColor.b, m_focusColor.a);
		break;
	default:
		m_needColor = glm::vec4(m_standartColor.r, m_standartColor.g, m_standartColor.b, m_standartColor.a);
		break;
	}
}
