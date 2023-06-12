#pragma once
#include"../include.h"
class ButtonAnimation {
public:
	ButtonAnimation(float time) {
		m_time = time;
	}

	void SetAnimateObject(Button* bt) {
		m_animateButton = bt;
	}

	void UpdateState() {
		m_state = AnimationState::End;
	}

	virtual void ProcessAnim() abstract;

	enum class AnimationState {
		Play,
		Stop,
		End
	};

protected:
	float m_time;


	AnimationState m_state;

	Button* m_animateButton;
};