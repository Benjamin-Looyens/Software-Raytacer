//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	// TODO : Implement GetFOV() function, where it checks if it needs to be calculated or not, otherwise just return stored value
	const float& fov = camera.GetFOV();
	const Matrix& camToWorld = camera.CalculateCameraToWorld();

	for (int px{}; px < m_Width; ++px) {
		for (int py{}; py < m_Height; ++py) {
			const float xNDC = (2 * ((px + 0.5f) / m_Width) - 1) * aspectRatio;
			const float yNDC = (1 - (2 * ((py + 0.5f) / m_Height)));

			// TODO : Check if NDC coordinates are necessary anywhere else, otherwise dont need to allocate a different variable just for them
			const float xC = xNDC * fov;
			const float yC = yNDC * fov;
			
			const Vector3 rayDir{ xC, yC, 1.0f };
			const Vector3 rayDir_Normal = camToWorld.TransformVector(rayDir.Normalized()); // TODO : prob switch up the camera transform elsewhere

			Ray viewRay{ camera.origin, rayDir_Normal };
			ColorRGB finalColor{ };
			HitRecord latestHit{ }; 

			pScene->GetClosestHit(viewRay, latestHit);

			if (latestHit.didHit) {
				for (const Light& light : pScene->GetLights()) {
					Ray lightRay{ latestHit.origin, light.origin - latestHit.origin, 0.001f };
					lightRay.max = lightRay.direction.Normalize();

					// NOTE : Can I calculate whether a light has any effect before the other calculations?
					// TODO : Point vs directional light difference?
					const Vector3 lightToTarget = light.origin - latestHit.origin;
					const Vector3 lightToTarget_Normal = lightToTarget.Normalized();
					const float observedArea = Vector3::Dot(latestHit.normal, lightToTarget_Normal);

					if (observedArea < 0) continue;
					if (m_DoShadows && pScene->DoesHit(lightRay)) continue;
					
					finalColor +=	materials[latestHit.materialIndex]->Shade(latestHit, lightToTarget_Normal, -viewRay.direction) *
									LightUtils::GetRadiance(light, latestHit.origin) * 
									std::clamp(observedArea, 0.0f, 1.0f);
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	m_CurMode = static_cast<LightingMode>((static_cast<int>(m_CurMode) + 1) % static_cast<int>(LightingMode::ModeAmount)); // Yuck
	//std::cout << "Current Lighting Mode: " << static_cast<int>(m_CurMode) << "\n";
}
