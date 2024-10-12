#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <GL/glew.h>

#include <vector>
#include <iostream>
#include <Windows.h>
using std::cout;
using std::endl;

union RGB
{ 
	float colors[3];
	struct
	{
		float r;
		float g;
		float b;
	};
};

struct Line
{
	std::vector<ImVec2> points;
	RGB color;
	float lineSize;
};

int main()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		return -1;
	}

	// Create SDL window
	SDL_Window* window = SDL_CreateWindow("ImGui SDL2 OpenGL3 Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window)
	{
		SDL_Quit();
		return -1;
	}

	// Create SDL GL context
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (!gl_context)
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	// Initialize OpenGL loader
	if (glewInit())
	{
		SDL_GL_DeleteContext(gl_context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	// Setup ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Main loop
	bool running = true;

	bool clearWindow = false;
	bool newLine = true;
	std::vector<Line> drawings;
	ImVec2 previousPoint = ImVec2(0, 0);
	int pointsAmount = 0;
	RGB currentColor = {255, 255, 255};
	float brushSize = 2.0f;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				running = false;
		}

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(970, 10));
		ImGui::SetNextWindowSize(ImVec2(300, 700));
		ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if (ImGui::Button("Clear Window"))
		{
			ImGui::Text("cleared");
			clearWindow = true;
			pointsAmount = 0;
			drawings.clear();
		}
		ImGui::SliderFloat("Brush Size", &brushSize, 0, 100);
		ImGui::ColorPicker3("Color Picker", currentColor.colors);
		if (ImGui::Button("Reset Color Values"))
		{
			currentColor.r = 255;
			currentColor.g = 255;
			currentColor.b = 255;
		}
		if (ImGui::Button("Reset Brush Size"))
		{
			brushSize = 2.0f;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Makes a screenshot of the window.");
			ImGui::EndTooltip();
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(950, 700));
		ImGui::Begin("Canvas", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
			ImGui::GetMousePos().x != previousPoint.x && ImGui::GetMousePos().y != previousPoint.y)
		{
			if (newLine)
			{
				Line line;
				line.color = currentColor;
				line.lineSize = brushSize;
				drawings.push_back(line);
				newLine = false;
			}
			drawings[drawings.size() - 1].points.push_back(ImGui::GetMousePos());
			previousPoint = ImGui::GetMousePos();
		}
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			newLine = true;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		for (Line line : drawings)
		{
			if (line.points.size() >= 2)
			{
				for (int i = 0; i < line.points.size() - 1; i++)
				{
					draw_list->PathLineTo(line.points[i]);
					//AddBezierCubic(line.points[i], line.points[i + 1], line.points[i + 3], line.points[i + 4], IM_COL32(line.color.r * 255, line.color.g * 255, line.color.b * 255, 255), line.lineSize);
				}
			draw_list->PathStroke(IM_COL32(line.color.r * 255, line.color.g * 255, line.color.b * 255, 255), 2, line.lineSize);
			}
		}
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		SDL_GetWindowSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}