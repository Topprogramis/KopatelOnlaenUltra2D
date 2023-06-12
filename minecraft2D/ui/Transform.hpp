#pragma once
#include "include.h"

class Transform {
public:
	Transform(glm::vec3 pos = glm::vec3(0.f),glm::vec3 rot = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f)) {
		m_pos = pos;
		m_rot = glm::quat(glm::radians(rot));
		m_scale = scale;
	}

	void Translate(glm::vec3 pos) {
		m_pos += pos;
		UpdateFlag();
	}
	void setPosition(glm::vec3 pos) {
		m_pos = pos;
		UpdateFlag();
	}
	glm::vec3 getPosition() {
		return m_pos;
	}

	void Rotate(glm::vec3 angle) {
		m_rot *= glm::quat(glm::radians(angle));
		UpdateFlag();
	}
	void RotateAdd(glm::vec3 angle) {
		m_rot += glm::quat(glm::radians(angle));
		UpdateFlag();
	}
	void SetRotation(glm::vec3 angle) {
		m_rot = glm::quat(glm::radians(angle));
		UpdateFlag();
	}
	glm::quat getRotation() {
		return m_rot;
	}

	void Scale(glm::vec3 scale) {
		m_scale += scale;
		UpdateFlag();
	}
	void setScale(glm::vec3 scale) {
		m_scale = scale;
		UpdateFlag();
	}
	glm::vec3 getScale() {
		return m_scale;
	}

	glm::mat4 getTransformMatrix() {
		if (m_updateFlag) {
			UpdateTransformMatrix();
			m_updateFlag = false;
		}

		return m_transformMatrix;
	}

private:
	glm::vec3 m_pos, m_scale;
	glm::quat m_rot;
	bool m_updateFlag;

	glm::mat4 m_transformMatrix;

	void UpdateTransformMatrix() {
		m_transformMatrix = glm::mat4(1.f);

		m_transformMatrix = glm::translate(m_transformMatrix, m_pos);
		m_transformMatrix *= glm::mat4_cast(m_rot);
		m_transformMatrix = glm::scale(m_transformMatrix, m_scale);
	}

	void UpdateFlag() {
		m_updateFlag = true;
	}
};