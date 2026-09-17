#include "App.h"
#include "HappyMath/Frustum.h"
#include "TestMeshSetOps.h"
#include "TestSurfaceUnion.h"
#include "TestPolygonCut.h"
#include <assert.h>

using namespace HappyMath;

App::App() : controller(0)
{
	this->window = nullptr;
	this->context = nullptr;
	this->appSetup = false;
	this->lastTickTime = 0;
	this->draggingMouse = false;
	this->testCase = nullptr;
}

/*virtual*/ App::~App()
{
	assert(this->testCase == nullptr);
}

bool App::Setup()
{
	if (this->appSetup)
		return false;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Failed to init video: %s", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	this->window = SDL_CreateWindow("Happy Math Test App", 2000, 1500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!this->window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	this->context = SDL_GL_CreateContext(this->window);
	if (!this->context)
	{
		SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
		return false;
	}

	SDL_GL_SetSwapInterval(1);	// Enable V-sync.

	//this->testCase = new TestSurfaceUnion();
	this->testCase = new TestMeshSetOps();
	//this->testCase = new TestPolygonCut();

	if (!this->testCase->Setup())
		return false;

	this->lastTickTime = SDL_GetTicksNS();

	this->camera = std::make_shared<Camera>();
	this->controller.AddButtonHandler(this->camera);

	this->appSetup = true;
	return true;
}

void App::Shutdown()
{
	if (this->testCase)
	{
		this->testCase->Shutdown();
		delete this->testCase;
		this->testCase = nullptr;
	}

	if (this->context)
	{
		SDL_GL_DestroyContext(this->context);
		this->context = nullptr;
	}

	if (this->window)
	{
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}

	SDL_Quit();
	this->appSetup = false;
}

bool App::Run()
{
	Uint64 currentTickTime = SDL_GetTicksNS();
	Uint64 deltaTicks = currentTickTime - this->lastTickTime;
	this->lastTickTime = currentTickTime;
	double deltaTimeSeconds = double(deltaTicks) / 1'000'000'000.0;

	// Prevent debugger pauses from being an issue.
	if (deltaTimeSeconds > 0.5)
		deltaTimeSeconds = 0.0;

	this->controller.Update();
	this->camera->Update(&this->controller);

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
			return false;

		this->HandleEvent(event, deltaTimeSeconds);
	}

	this->Render(deltaTimeSeconds);

	return true;
}

void App::Render(double deltaTimeSeconds)
{
	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	double aspectRatio = double(windowWidth) / double(windowHeight);

	HappyMath::Frustum frustum;
	frustum.SetFromAspectRatio(aspectRatio, M_PI / 3.0, 0.1, 1000.0);

	HappyMath::Matrix4x4 projMatrix;
	frustum.GetToProjectionMatrix(projMatrix);

	HappyMath::Matrix4x4 projMatrixT;
	projMatrixT.Transpose(projMatrix);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixd(&projMatrixT.ele[0][0]);

	HappyMath::Matrix4x4 viewToWorld;
	this->camera->MakeViewToWorldMatrix(viewToWorld);

	HappyMath::Matrix4x4 worldToView;
	worldToView.Invert(viewToWorld);

	HappyMath::Matrix4x4 worldToViewT;
	worldToViewT.Transpose(worldToView);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(&worldToViewT.ele[0][0]);

	glBegin(GL_LINES);

	// X-axis.
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	// Y-axis.
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	// Z-axis.
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glEnd();

	if (this->testCase)
		this->testCase->Render();

	SDL_GL_SwapWindow(this->window);
}

void App::HandleEvent(SDL_Event& event, double deltaTimeSeconds)
{
#if 0
	switch (event.type)
	{
		case SDL_EVENT_MOUSE_MOTION:
		{
			if (this->draggingMouse)
			{
				Vector2 currentMousePos(event.motion.x, event.motion.y);
				Vector2 mouseDelta = currentMousePos - this->lastMousePos;
				this->lastMousePos = currentMousePos;

				double distanceToOrigin = this->cameraEye.Length();

				Vector3 xAxis, yAxis, zAxes;
				this->viewToWorld.GetAxes(xAxis, yAxis, zAxes);

				double mouseSensativity = 0.03;

				this->cameraEye += mouseSensativity * (-mouseDelta.x * xAxis + mouseDelta.y * yAxis);
				this->cameraEye *= distanceToOrigin / this->cameraEye.Length();
			}

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				this->draggingMouse = true;
				this->lastMousePos.x = event.motion.x;
				this->lastMousePos.y = event.motion.y;
			}
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (event.button.button == SDL_BUTTON_LEFT)
				this->draggingMouse = false;
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			double distanceToOrigin = this->cameraEye.Length();

			double zoomFactor = (event.wheel.y > 0.0f) ? 0.9 : 1.1;

			for (int i = 0; i < (int)::fabs(event.wheel.y); i++)
				distanceToOrigin *= zoomFactor;

			this->cameraEye *= distanceToOrigin / this->cameraEye.Length();

			break;
		}
	}
#endif
}

//----------------------------------- App::Camera -----------------------------------

App::Camera::Camera()
{
	this->eye.SetComponents(0.0, 0.0, 40.0);
	this->unitLookDir.SetComponents(0.0, 0.0, -1.0);
	this->strafePlane = StrafePlane::XZ;
}

void App::Camera::MakeViewToWorldMatrix(HappyMath::Matrix4x4& viewToWorld) const
{
	HappyMath::Vector3 cameraUp(0.0, 1.0, 0.0);

	viewToWorld.SetAsViewToWorldTransform(this->eye, this->eye + this->unitLookDir, cameraUp);
}

void App::Camera::Update(XBoxController* controller)
{
	// STPTODO: Use delta time here instead of sensativity variables.

	HappyMath::Vector2 leftThumbStick = controller->GetAnalogJoyStick(XINPUT_GAMEPAD_LEFT_THUMB);
	HappyMath::Vector2 rightThumbStick = controller->GetAnalogJoyStick(XINPUT_GAMEPAD_RIGHT_THUMB);

	HappyMath::Matrix4x4 viewToWorld;
	this->MakeViewToWorldMatrix(viewToWorld);

	HappyMath::Vector3 xAxis, yAxis, zAxis;
	viewToWorld.GetAxes(xAxis, yAxis, zAxis);

	double strafeSensativity = 0.1;

	HappyMath::Vector3 eyeDelta;

	switch (this->strafePlane)
	{
	case StrafePlane::XY:
		eyeDelta = strafeSensativity * (xAxis * leftThumbStick.x + yAxis * leftThumbStick.y);
		break;
	case StrafePlane::XZ:
		eyeDelta = strafeSensativity * (xAxis * leftThumbStick.x - zAxis * leftThumbStick.y);
		break;
	}

	this->eye += eyeDelta;

	double lookSensativity = 0.02;

	HappyMath::Vector3 lookDirDelta = lookSensativity * (xAxis * rightThumbStick.x + yAxis * rightThumbStick.y);

	this->unitLookDir += lookDirDelta;
	this->unitLookDir.Normalize();
}

/*virtual*/ void App::Camera::OnButtonPressed(DWORD button)
{
}

/*virtual*/ void App::Camera::OnButtonReleased(DWORD button)
{
	if (button == XINPUT_GAMEPAD_A)
	{
		switch (this->strafePlane)
		{
		case StrafePlane::XY:
			this->strafePlane = StrafePlane::XZ;
			break;
		case StrafePlane::XZ:
			this->strafePlane = StrafePlane::XY;
			break;
		}
	}
}

/*virtual*/ void App::Camera::OnButtonDown(DWORD button)
{
}

/*virtual*/ void App::Camera::OnButtonUp(DWORD button)
{
}