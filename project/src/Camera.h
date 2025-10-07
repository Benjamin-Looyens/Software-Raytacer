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
		float fov{ };
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

			// Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			Vector3 movement{};
			Vector3 rotation{};

			bool movDirty{};
			bool rotDirty{};

			if (pKeyboardState[SDL_SCANCODE_W]) { movement += forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_S]) { movement -= forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_D]) { movement += right; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_A]) { movement -= right; movDirty = true; }

			if (pKeyboardState[SDL_SCANCODE_SPACE]) { movement += up; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_LSHIFT]) { movement -= up; movDirty = true; }

			// Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool rDown = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
			const bool lDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

			// NOTE : potentially could have a check for if there is keyboard input so that it does not double up, HOWEVER I normalize anyways so it should be fine
			if (rDown && lDown) {
				movement.y -= mouseY;
				movDirty = true;
			}
			else if (rDown) {
				// TODO : Fix inverted up/down controls when flipped 180?
				rotation.y += mouseX;
				rotation.x -= mouseY; 
				rotDirty = true;
			}
			else if (lDown) {
				// TODO : clamp forward movement?
				movement -= forward * static_cast<float>(mouseY);
				rotation.y += mouseX;

				movDirty = true;
				rotDirty = true;
			}

			if (movDirty && movement.Normalize() > 0.0f) {
				// TODO : Figure out if this would work with the LMB movement when going into the scene
				movement.Normalize();
				
				origin += movement * movSpeed * deltaTime;
				toWorldDirty = true;
			}
			if (rotDirty && rotation.Magnitude() > 0.0f) {
				// TODO : Actually use totalPitch & totalYaw
				const Matrix rotMatrix = Matrix::CreateRotation(rotation * rotSpeed * deltaTime);
				forward = rotMatrix.TransformVector(forward);
				forward.Normalize(); // Is this necessary here?
				toWorldDirty = true;
			}
		}
	};
}
