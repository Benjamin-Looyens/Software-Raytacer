//External includes
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>
#include <vector>

//Project includes
#include "Timer.h"
#include "Renderer.h"
#include "Scene.h"
#if defined(_DEBUG)
#include "LeakDetector.h"
#endif

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	// Leak detection
	#if defined(_DEBUG)
		LeakDetector detector{};
	#endif

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - **Benjamin Looyens 2DAE11**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	std::vector<Scene*> pScenes{ new Scene_W4_Reference(), new Scene_W4_Bunny() };
	size_t curSceneIdx = 0;

	// HACK : Can I reuse some stuff from the scenes?
	// For example maybe I can reuse camera?
	for (Scene* pScene : pScenes)
		pScene->Initialize();

	//Start loop
	pTimer->Start();

	// Start Benchmark
	// pTimer->StartBenchmark();

	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		const auto& pCurScene = pScenes[curSceneIdx];

		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
					pRenderer->ToggleShadows();
				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
					pRenderer->CycleLightingMode();
				if (e.key.keysym.scancode == SDL_SCANCODE_Z)
					curSceneIdx = (curSceneIdx + 1) % pScenes.size();
				if (e.key.keysym.scancode == SDL_SCANCODE_R) {
					for (Scene* pScene : pScenes) {
						const auto& pRotScene = dynamic_cast<RotatingMeshScene*>(pScene);
						pRotScene->ToggleRotation();
					}
				}
				break;
			}
		}

		//--------- Update ---------
		pCurScene->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render(pCurScene);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	for (Scene* pScene : pScenes)
		delete pScene;
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}