#include"ui/Animator.hpp"
#include"Program.hpp"

Animator::Animator(IAnimatable* animateObject) {
	m_animateObject = animateObject;
	Settings::program->AddAnimatorObject(this);
}