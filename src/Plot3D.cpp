#include "Plot3D.hpp"

#include <optional>
#include <array>
#include <iostream>

#include "backend/Camera.hpp"
#include "backend/ObjectManager.hpp"
#include "Util.hpp"

inline float Map(const glm::vec2& from, const glm::vec2& to, float val)
{
	return (val - from.x) * (to.y - to.x) / (from.y - from.x) + to.x;
}

Plot3D::Plot3D(const BBox& domainAndRange, float scale, float resolution, PlottableFunction func)
{
	// magic epsilon
	if (resolution < 0.0001f) return;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	std::vector<std::optional<unsigned int>> functionValues;
	unsigned int sliceLength = 0;
	unsigned int slices = 0;

	// Intervals of the domain and (desired) range
	glm::vec2 xInterval(domainAndRange.x, domainAndRange.w);
	glm::vec2 yInterval(domainAndRange.y, domainAndRange.h);
	glm::vec2 zInterval(domainAndRange.z, domainAndRange.d);

	// Bounding box of the graph in 3D space (centered at (0, 0, 0)
	glm::vec2 xBounds = glm::vec2(-0.5f, 0.5f) * ((xInterval.y - xInterval.x) * scale);
	glm::vec2 yBounds = glm::vec2(-0.5f, 0.5f) * ((yInterval.y - yInterval.x) * scale);

	// Calculate function values
	unsigned int index = 0;
	float minFunctionValue = std::numeric_limits<float>::max();
	float maxFunctionValue = std::numeric_limits<float>::min();

	for (float y = yInterval.x; y <= yInterval.y; y += resolution)
	{
		sliceLength = 0;

		for (float x = xInterval.x; x <= xInterval.y; x += resolution)
		{
			if (std::abs(x) < 0.05f && std::abs(y) < 0.05f)
				volatile int sjdks = 3;

			float val = func(x, y);
			std::optional<unsigned int> pointIndex;
			
			// If function value is in bbox
			if (val >= zInterval.x && val <= zInterval.y)
			{
				vertices.push_back(Map(xInterval, xBounds, x));
				vertices.push_back(val);	// Will be corrected later!
				vertices.push_back(Map(yInterval, yBounds, y));
				vertices.push_back(0.0f);

				minFunctionValue = std::min(minFunctionValue, val);
				maxFunctionValue = std::max(maxFunctionValue, val);

				pointIndex = index;
				index++;
			}

			functionValues.push_back(pointIndex);
			sliceLength++;
		}

		slices++;
	}

	zInterval = glm::vec2(minFunctionValue, maxFunctionValue);
	glm::vec2 zBounds = glm::vec2(-0.5f, 0.5f) * ((zInterval.y - zInterval.x) * scale);

	for (unsigned int i = 3; i < vertices.size(); i += 4)
	{
		vertices[i] = Map(zInterval, glm::vec2(0.0f, 1.0f), vertices[i - 2]);
		vertices[i - 2] = Map(zInterval, zBounds, vertices[i - 2]);
	}

	/*
	*   This is what the desired meshing should look like
	*                                                                 +y
	*   CurrentSlice    o---o---o---o---o---o---o---o   x   o          ^
	*	                | /   \ | / | / | /       \ | \   / |          |
	*	PreviousSlice   o   x   o---o---o   x   x   o---o---o          +---> +x
	*
	* 
	*	Possible fragment constellations
	*   +---------+---------+---------+---------+---------+---------+
	*   |  2---1  |  o---o  |  o---o  |  x   o  |  o---o  |  o   x  |
	*   |  |   |  |  | / |  |  | /    |    / |  |    \ |  |  | \    |
	*   |  3---4  |  o---o  |  o   x  |  o---o  |  x   o  |  o---o  |  
	*   +---------+---------+---------+---------+---------+---------+
	* 
	*	                     123,      134,      124,      234
	* 
	* 	o = Function value inside bbox
	*   x = Function value outside bbox
	*
	*   By default OpenGL treats triangles defined in couter-clockwise order as "front facing". So the algorithm
	*   should construct the triangles in that way (even if we're not culling faces, just to not have any funny bugs in the future)
	*
	*   The algorithm works as follows:
	*	 1. Pick a patch of 4 points from the value list, and label them like in the diagram above
	*    2. Attempt to connect the 123 constellation.
	*    3. Attempt to connect the next constellation, until none are left
	*      -> Success: Go to 1
	*      -> Failure: Go to 3
	*/

	struct Constellation {
		unsigned int i, j, k;
	};

	std::array<Constellation, 4> constellations = {
		Constellation {0, 1, 2},
		Constellation {0, 2, 3},
		Constellation {0, 1, 3},
		Constellation {1, 2, 3}
	};
	std::array<std::optional<unsigned int>*, 4> points;

	for (unsigned int y = 1; y < slices; y++)
	{
		for (unsigned int x = 1; x < sliceLength; x++)
		{
			points = { 
				&functionValues[y * sliceLength + x], 
				&functionValues[y * sliceLength + (x - 1)], 
				&functionValues[(y - 1) * sliceLength + (x - 1)], 
				&functionValues[(y - 1) * sliceLength + x], 
			};

			int matches = 0;
			for (Constellation& constellation : constellations)
			{
				// This constellation doesnt match
				if (!(points[constellation.i]->has_value() && points[constellation.j]->has_value() && points[constellation.k]->has_value()))
					continue;

				indices.push_back(points[constellation.i]->value());
				indices.push_back(points[constellation.j]->value());
				indices.push_back(points[constellation.k]->value());

				matches++;
				if (matches == 2)
					break;
			}
		}
	}

	vao = VAOFactory::Produce(vertices, indices, 
		{
			{ 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0 },
			{ 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(3 * sizeof(float))}
		}
	);

	shader = ShaderManager::GetInstance().Get(PLOT3D_ID);
	if (shader == nullptr)
	{
		shader = ShaderFactory::Produce(
			R"(
				#version 440 core
			
				layout (location = 0) in vec3 aPosition;
				layout (location = 1) in float aU;

				out float u;

				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;

				void main()
				{
					u = aU;
					gl_Position = projection * view * model * vec4(aPosition, 1.0f);
				}
			)",
				R"(
				#version 440 core

				in float u;

				out vec4 FragColor;

				void main()
				{
					FragColor = vec4(1.0f - u, 0.0f, u, 1.0f);
				}
			)"
		);

		ShaderManager::GetInstance().Register(PLOT3D_ID, shader);
	}
}

void Plot3D::PreRender(const CameraBase& camera) const
{
	shader->SetUniform("model", transformation);
	shader->SetUniform("view", camera.GetView());
	shader->SetUniform("projection", camera.GetProjection());
}
