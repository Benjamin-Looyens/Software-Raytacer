//Defines
#define PARALLEL_EXECUTION

//Standard includes
#ifdef PARALLEL_EXECUTION
#include <execution>
#endif

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

	const float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	const float& fov = camera.GetFOV();
	const Matrix& camToWorld = camera.CalculateCameraToWorld();

	const uint32_t pixelCount{ static_cast<uint32_t>(m_Width * m_Height) };

#ifdef PARALLEL_EXECUTION
		std::vector<uint32_t> pixelIndices{};
		pixelIndices.reserve(pixelCount);
		for (uint32_t index{}; index < pixelCount; ++index) pixelIndices.emplace_back(index);

		std::for_each(std::execution::par, begin(pixelIndices), end(pixelIndices),
			[&](const uint32_t pixel) { RenderPixel(pScene, pixel, fov, aspectRatio, camToWorld, camera.origin); }
		);
#else
	for (uint32_t pixel{}; pixel < pixelCount; ++pixel)
		RenderPixel(pScene, pixel, fov, aspectRatio, camToWorld, camera.origin);
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void dae::Renderer::RenderPixel(const Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix& camToWorld, const Vector3& cameraOrigin) const
{
	const auto& materials = pScene->GetMaterials();
	const auto& lights = pScene->GetLights();

	const uint32_t px{ pixelIndex % m_Width };
	const uint32_t py{ pixelIndex / m_Width };

	const float xC = ((2 * ((static_cast<float>(px) + 0.5f) / m_Width) - 1) * aspectRatio) * fov;
	const float yC = ((1 - (2 * ((static_cast<float>(py) + 0.5f) / m_Height)))) * fov;

	const Vector3 rayDir{ xC, yC, 1.0f };
	const Vector3 rayDir_Normal = camToWorld.TransformVector(rayDir.Normalized()); // TODO : prob switch up the camera transform elsewhere

	const Ray viewRay{ cameraOrigin, rayDir_Normal };
	ColorRGB finalColor{ };
	HitRecord latestHit{ };

	pScene->GetClosestHit(viewRay, latestHit);

	if (latestHit.didHit) {
		for (const Light& light : lights) {
			// TODO : Point vs directional light difference?
			Vector3 lightDir = light.origin - latestHit.origin;
			const float lightMagnitude = lightDir.Normalize();
			const Ray lightRay{ latestHit.origin, lightDir, 0.001f, lightMagnitude };

			if (m_DoShadows && pScene->DoesHit(lightRay)) continue;

			const float observedArea = std::clamp(Vector3::Dot(latestHit.normal, lightDir), FLT_MIN, 1.0f);
			if (observedArea < 0.0f) continue;

			switch (m_CurMode) {
			case LightingMode::ObservedArea:
				finalColor += { observedArea, observedArea, observedArea };
				break;
			case LightingMode::Radiance:
				// TODO : Fix the fact that there still seems to be some sort of shadows on the spheres themselves even with shadows off
				finalColor += LightUtils::GetRadiance(light, latestHit.origin);
				break;
			case LightingMode::BRDF:
				// TODO : Fix the rendering on bottom spheres (look at slides)
				finalColor += materials[latestHit.materialIndex]->Shade(latestHit, lightDir, -viewRay.direction);
				break;
			case LightingMode::Combined:
				finalColor += materials[latestHit.materialIndex]->Shade(latestHit, lightDir, -viewRay.direction) *
					LightUtils::GetRadiance(light, latestHit.origin) *
					observedArea;
				break;
			}
		}
	}

	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::ToggleShadows()
{
	m_DoShadows = !m_DoShadows;

	std::cout << "Shadows have been turned ";
	if (m_DoShadows) std::cout << "on.\n";
	else std::cout << "off.\n";
}

void dae::Renderer::CycleLightingMode()
{
	// TODO : Implement a smooth way to print out the fact lighting mode changed
	m_CurMode = static_cast<LightingMode>((static_cast<uint8_t>(m_CurMode) + 1) % static_cast<uint8_t>(LightingMode::ModeAmount)); // Yuck
}

