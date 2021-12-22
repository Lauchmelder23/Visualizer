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

/**
 * Creates the list of vertices for this plot.
 * 
 * @param[in]  domainAndRange The domain in which to plot the function and the maximum allowed range (values outside are clipped)
 * @param[in]  scale          Scale of the plot
 * @param[in]  resolution     Step size in which to scan over the function
 * @param[in]  func           Function to plot
 * @param[out] functionValues Values of the function at each point (if any)
 * @param[out] vertices       Vertices of the function
 * 
 * @return Number of vertices per slice in the plot
 */
unsigned int CreateVertexList(const BBox& domainAndRange, float scale, float resolution, PlottableFunction func, std::vector<std::optional<unsigned int>>& functionValues, std::vector<float>& vertices);

/**
 * Triangulate the set of vertices to create the plot
 * 
 * @param[in]  functionValues Values of the function at each point (if any)
 * @param[in]  sliceLength    Number of vertices per slice in the plot
 * @param[out] indices        The indices for the mesh
 */
void Triangulate(const std::vector<std::optional<unsigned int>>& functionValues, unsigned int sliceLength, std::vector<unsigned int>& indices);

Plot3D::Plot3D(const BBox& domainAndRange, float scale, float resolution, PlottableFunction func)
{
	// magic epsilon
	if (resolution < 0.0001f) return;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	std::vector<std::optional<unsigned int>> functionValues;

	unsigned int sliceLength = CreateVertexList(domainAndRange, scale, resolution, func, functionValues, vertices);
	Triangulate(functionValues, sliceLength, indices);

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

unsigned int CreateVertexList(const BBox& domainAndRange, float scale, float resolution, PlottableFunction func, std::vector<std::optional<unsigned int>>& functionValues, std::vector<float>& vertices)
{
	unsigned int sliceLength = 0;

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

	// Loop over the function and query function values at each point
	for (float y = yInterval.x; y <= yInterval.y; y += resolution)
	{
		sliceLength = 0;	// Length of the current slice

		for (float x = xInterval.x; x <= xInterval.y; x += resolution)
		{
			float val = func(x, y);
			std::optional<unsigned int> pointIndex;

			// If function value is in bbox
			if (val >= zInterval.x && val <= zInterval.y)
			{
				// Set vertex x and y coordinate
				// z is still left in "function space"
				// The fourth value is later gonna be the "normalized z", it is set to 0.0 here to avoid resizing and inserting into the vector later
				vertices.push_back(Map(xInterval, xBounds, x));
				vertices.push_back(val);	// Will be corrected later!
				vertices.push_back(Map(yInterval, yBounds, y));
				vertices.push_back(0.0f);

				// Update the min/max vals
				minFunctionValue = std::min(minFunctionValue, val);
				maxFunctionValue = std::max(maxFunctionValue, val);

				pointIndex = index;
				index++;
			}

			// Insert the value of this function into the function value array
			functionValues.push_back(pointIndex);
			sliceLength++;
		}

	}

	// Update z interval, so the function is scaled properly later
	zInterval = glm::vec2(minFunctionValue, maxFunctionValue);
	glm::vec2 zBounds = glm::vec2(-0.5f, 0.5f) * ((zInterval.y - zInterval.x) * scale);

	for (unsigned int i = 3; i < vertices.size(); i += 4)
	{
		// Go over array and replace the normalized z with the actual value, and update z
		vertices[i] = Map(zInterval, glm::vec2(0.0f, 1.0f), vertices[i - 2]);
		vertices[i - 2] = Map(zInterval, zBounds, vertices[i - 2]);
	}

	return sliceLength;
}

void Triangulate(const std::vector<std::optional<unsigned int>>& functionValues, unsigned int sliceLength, std::vector<unsigned int>& indices)
{

	/*
	* This is what the desired meshing should look like
	*                                                               +y
	* CurrentSlice    o---o---o---o---o---o---o---o   x   o          ^
	*                 | /   \ | / | / | /       \ | \   / |          |
	* PreviousSlice   o   x   o---o---o   x   x   o---o---o          +---> +x
	*
	*
	* Possible fragment constellations
	* +---------+---------+---------+---------+---------+---------+
	* |  2---1  |  o---o  |  o---o  |  x   o  |  o---o  |  o   x  |
	* |  |   |  |  | / |  |  | /    |    / |  |    \ |  |  | \    |
	* |  3---4  |  o---o  |  o   x  |  o---o  |  x   o  |  o---o  |
	* +---------+---------+---------+---------+---------+---------+
	*
	*                      123,      134,      124,      234
	*
	* o = Function value inside bbox
	* x = Function value outside bbox
	*
	* By default OpenGL treats triangles defined in couter-clockwise order as "front facing". So the algorithm
	* should construct the triangles in that way (even if we're not culling faces, just to not have any funny bugs in the future)
	*
	* The algorithm works as follows:
	*  1. Pick a patch of 4 points from the value list, and label them like in the diagram above
	*  2. Attempt to connect the 123 constellation.
	*  3. Attempt to connect the next constellation, until none are left
	*    -> Success: Go to 1
	*    -> Failure: Go to 3
	*/

	struct Constellation {
		unsigned int i, j, k;
	};

	// Possible constellations
	std::array<Constellation, 4> constellations = {
		Constellation {0, 1, 2},
		Constellation {0, 2, 3},
		Constellation {0, 1, 3},
		Constellation {1, 2, 3}
	};
	std::array<const std::optional<unsigned int>*, 4> points;

	// Loop over vertices, starting at (1, 1)
	for (unsigned int y = 1; y * sliceLength < functionValues.size(); y++)
	{
		for (unsigned int x = 1; x < sliceLength; x++)
		{
			// "Overlay the square", basically assigns a number to the vertices according to the diagram above
			points = {
				&functionValues[y * sliceLength + x],
				&functionValues[y * sliceLength + (x - 1)],
				&functionValues[(y - 1) * sliceLength + (x - 1)],
				&functionValues[(y - 1) * sliceLength + x],
			};

			// Try and match the first two constellations
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
}
