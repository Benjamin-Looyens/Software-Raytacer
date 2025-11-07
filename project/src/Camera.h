#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include "Math.h"
#include "Timer.h"
#include <iostream>

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
		float rotSpeed{ 0.2f };

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

			Vector3 movement{};
			Vector3 rotation{};

			bool movDirty{};
			bool rotDirty{};

			// Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP]) { movement += forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN]) { movement -= forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT]) { movement += right; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT]) { movement -= right; movDirty = true; }

			if (pKeyboardState[SDL_SCANCODE_SPACE]) { movement += Vector3::UnitY; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_LSHIFT]) { movement -= Vector3::UnitY; movDirty = true; }

			// Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool rDown = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
			const bool lDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

			if (rDown && lDown) {
				movement.y -= mouseY;
				movDirty = true;
			}
			else if (rDown) {
				rotation.y += mouseX;
				rotation.x -= mouseY; 
				rotDirty = true;
			}
			else if (lDown) {
				movement -= forward * static_cast<float>(mouseY);
				rotation.y += mouseX;

				movDirty = true;
				rotDirty = true;
			}

			if (movDirty && movement.Normalize() > 0.0f) { // NOTE : Ideally, the mouse movement shouldn't be normalized but making a bool just for that seems messy
				origin += movement * movSpeed * deltaTime;
				toWorldDirty = true;
			}
			if (rotDirty && rotation.Magnitude() > 0.0f) { // NOTE : In theory I could do SqrMagnitude?
				const Vector3 scaledRotation = rotation * rotSpeed * deltaTime;

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
