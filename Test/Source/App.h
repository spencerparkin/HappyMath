#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "HappyMath/Vector3.h"
#include "HappyMath/Vector2.h"
#include "HappyMath/Matrix4x4.h"
#include "XBoxController.h"

class TestCase;

/**
 * 
 */
class App
{
public:
	App();
	virtual ~App();

	bool Setup();
	void Shutdown();
	bool Run();

private:
	void Render(double deltaTimeSeconds);
	void HandleEvent(SDL_Event& event, double deltaTimeSeconds);

	class Camera : public XBoxControllerButtonHandler
	{
	public:
		Camera();

		void MakeViewToWorldMatrix(HappyMath::Matrix4x4& viewToWorld) const;
		void Update(XBoxController* controller);

		virtual void OnButtonPressed(DWORD button) override;
		virtual void OnButtonReleased(DWORD button) override;
		virtual void OnButtonDown(DWORD button) override;
		virtual void OnButtonUp(DWORD button) override;

		enum StrafePlane
		{
			XY,
			XZ
		};

		HappyMath::Vector3 eye;
		HappyMath::Vector3 unitLookDir;
		StrafePlane strafePlane;
	};

	SDL_Window* window;
	SDL_GLContext context;
	bool appSetup;
	TestCase* testCase;
	Uint64 lastTickTime;
	bool draggingMouse;
	HappyMath::Vector2 lastMousePos;
	std::shared_ptr<Camera> camera;
	XBoxController controller;
};