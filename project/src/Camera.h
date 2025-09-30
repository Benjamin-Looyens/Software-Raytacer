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

		float cameraSpeed{ 10.0f };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};
		bool toWorldDirty{ true };

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

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			Vector3 movement{};

			if (pKeyboardState[SDL_SCANCODE_W]) {
				movement += forward;
				toWorldDirty = true;
			}
			if (pKeyboardState[SDL_SCANCODE_S]) {
				movement -= forward;
				toWorldDirty = true;
			}
			if (pKeyboardState[SDL_SCANCODE_D]) {
				movement += right;
				toWorldDirty = true;
			}
			if (pKeyboardState[SDL_SCANCODE_A]) {
				movement -= right;
				toWorldDirty = true;
			}

			if (movement.Normalize() > 0.0f) origin += movement * cameraSpeed * deltaTime;

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			
			// TODO : set toWorldDirty to true if camera changed
		}
	};
}
