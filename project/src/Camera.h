#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera final
	{
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{}

		Vector3 origin{};
		float fovAngle{ 90.f };
		float fov{ }; // NOTE : Camera is a struct, so would assume POD, but could set as private maybe?
		bool fovDirty{ true };

		float movSpeed{ 10.0f };
		float panSpeed{ 0.25f };
		float rotSpeed{ 0.0075f };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};
		bool toWorldDirty{ true };

		float GetFOV() 
		{
			if (fovDirty) {
				fov = tanf((fovAngle / 2) * TO_RADIANS);
				fovDirty = false;
			}

			return fov;
		}

		Matrix CalculateCameraToWorld()
		{
			if (toWorldDirty) {
				right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
				up = Vector3::Cross(forward, right).Normalized();	
				cameraToWorld = { right, up, forward, origin };
				toWorldDirty = false;
			}

			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			Vector3 movementMouse{};
			Vector3 movementKeyboard{};
			Vector3 rotation{};

			bool movDirty{};
			bool rotDirty{};
			float sprintModifier{ 1.0f };

			// Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP]) { movementKeyboard += forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN]) { movementKeyboard -= forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT]) { movementKeyboard += right; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT]) { movementKeyboard -= right; movDirty = true; }

			if (pKeyboardState[SDL_SCANCODE_SPACE] || pKeyboardState[SDL_SCANCODE_E]) { movementKeyboard += Vector3::UnitY; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_LCTRL] || pKeyboardState[SDL_SCANCODE_Q]) { movementKeyboard -= Vector3::UnitY; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_LSHIFT]) sprintModifier = 2.0f;

			// Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool rDown = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
			const bool lDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

			if (rDown && lDown) {
				movementMouse.y -= mouseY;
				movementMouse += right * static_cast<float>(mouseX);
				movDirty = true;
			}
			else if (rDown) {
				rotation.y += mouseX;
				rotation.x -= mouseY;
				rotDirty = true;
			}
			else if (lDown) {
				movementMouse -= forward * static_cast<float>(mouseY);
				rotation.y += mouseX;

				movDirty = true;
				rotDirty = true;
			}

			if (movDirty) {
				if (movementKeyboard.Normalize() > 0.0f) origin += movementKeyboard * movSpeed * deltaTime * sprintModifier;
				if (movementMouse.SqrMagnitude() > 0.0f) origin += movementMouse * panSpeed;
				toWorldDirty = true;
			}
			if (rotDirty && rotation.SqrMagnitude() > 0.0f) {
				const Vector3 scaledRotation = rotation * rotSpeed;

				constexpr float OFFSET = 0.05f;
				totalYaw += scaledRotation.x;
				totalYaw = std::clamp(totalYaw, -PI * 0.5f + OFFSET, PI * 0.5f - OFFSET);

				// Ensure always positive between 0 -> 6.28...
				totalPitch = std::fmodf(std::fmodf(scaledRotation.y + totalPitch, PI_2) + PI_2, PI_2);

				const Matrix fullRotMatrix = Matrix::CreateRotationX(totalYaw) * Matrix::CreateRotationY(totalPitch);
				forward = fullRotMatrix.TransformVector(Vector3::UnitZ);
				toWorldDirty = true;
			}
		}
	};
}
