#pragma once
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera {

	private:
		float m_yaw;
		float m_pitch;
		float m_fov;
		float m_speed; // unit per second
		float m_sensitivity;

		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;

		void updateCameraVectors();

	public:
		// constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
			: m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_speed(SPEED), m_sensitivity(SENSITIVITY), m_fov(FOV) {
			m_position = position;
			m_worldUp = up;
			m_yaw = yaw;
			m_pitch = pitch;
			updateCameraVectors();
		}

		// constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
			: m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_speed(SPEED), m_sensitivity(SENSITIVITY), m_fov(FOV) {
			m_position = glm::vec3(posX, posY, posZ);
			m_worldUp = glm::vec3(upX, upY, upZ);
			m_yaw = yaw;
			m_pitch = pitch;
			updateCameraVectors();
		}

		void keyProcess(CameraMovement direction, float deltaTime);
		void mouseProcess(double xoffset, double yoffset);
		void scrollProcess(double yoffset);

		inline glm::mat4 getViewMatrix() const {
			return glm::lookAt(m_position, m_position + m_front, m_up);
		};

		inline float getFov() const {
			return m_fov;
		}

		inline glm::vec3 getPosition() const {
			return m_position;
		}

};