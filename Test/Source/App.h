#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "HappyMath/PolygonMesh.h"
#include "HappyMath/Graph.h"
#include "HappyMath/Vector3.h"
#include "HappyMath/Vector2.h"
#include "HappyMath/Matrix4x4.h"

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

	SDL_Window* window;
	SDL_GLContext context;
	bool appSetup;
	std::set<HappyMath::Graph::UnorderedEdge, HappyMath::Graph::UnorderedEdge> edgeSet;
	HappyMath::Graph graph;
	HappyMath::PolygonMesh mesh;
	HappyMath::Vector3 cameraEye;
	Uint64 lastTickTime;
	bool draggingMouse;
	HappyMath::Vector2 lastMousePos;
	HappyMath::Matrix4x4 viewToWorld;
};