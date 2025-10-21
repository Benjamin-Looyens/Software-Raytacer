#pragma once
#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	struct Matrix;
	struct Vector3;

	class Scene;
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		void RenderPixel(const Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix& cameraToWorld, const Vector3& cameraOrigin) const; // Cant I just pass the camera?
		bool SaveBufferToImage() const;
		void ToggleShadows();
		void CycleLightingMode();

	private:
		enum class LightingMode : uint8_t
		{
			ObservedArea,
			Radiance,
			BRDF,
			Combined,
			ModeAmount
		};


		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		bool m_DoShadows{ true };
		LightingMode m_CurMode{ LightingMode::Combined };
	};
}
