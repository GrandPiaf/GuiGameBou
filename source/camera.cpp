#include "Camera.h"

void Camera::updateCameraVectors() {

	// calculate the new Front vector
	glm::vec3 direction;
	direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	direction.y = sin(glm::radians(m_pitch));
	direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(direction);

	// also re-calculate the Right and Up vector
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_up = glm::normalize(glm::cross(m_right, m_front));

}

void Camera::keyProcess(CameraMovement direction, float deltaTime) {
	// Movements
	float cameraVelocity = m_speed * deltaTime; // adjust accordingly
	if (direction == CameraMovement::FORWARD) {
		m_position += m_front * cameraVelocity;
	}
	if (direction == CameraMovement::BACKWARD) {
		m_position -= m_front * cameraVelocity;
	}
	if (direction == CameraMovement::LEFT) {
		m_position -= m_right * cameraVelocity;
	}
	if (direction == CameraMovement::RIGHT) {
		m_position += m_right * cameraVelocity;
	}
}

void Camera::mouseProcess(double xoffset, double yoffset) {

	xoffset *= m_sensitivity;
	yoffset *= m_sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	// Blocking player view to avoid lookAt flip
	if (m_pitch > 89.0f) {
		m_pitch = 89.0f;
	}
	if (m_pitch < -89.0f) {
		m_pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::scrollProcess(double yoffset) {
	m_fov -= (float)yoffset;
	if (m_fov < 1.0f) {
		m_fov = 1.0f;
	}
	if (m_fov > 45.0f) {
		m_fov = 45.0f;
	}
}