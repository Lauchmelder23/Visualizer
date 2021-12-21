#include "Grid.hpp"

#include <stdexcept>
#include <vector>

#include "backend/Camera.hpp"
#include "backend/ObjectManager.hpp"
#include "Util.hpp"

Grid::Grid(const glm::vec2& size, unsigned int linesAlongWidth, unsigned int linesAlongHeight)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	glm::vec2 halfSize = size / 2.0f;

	// Unoptimized memory usage
	for (unsigned int x = 0; x <= linesAlongWidth; x++)
	{
		vertices.push_back(-halfSize.x + (float)x * size.x / (float)linesAlongWidth);
		vertices.push_back(-halfSize.y);

		vertices.push_back(-halfSize.x + (float)x * size.x / (float)linesAlongWidth);
		vertices.push_back(halfSize.y);

		indices.push_back(2 * x);
		indices.push_back(2 * x + 1);
	}

	unsigned int lastIndex = indices.back() + 1;
	for (unsigned int y = 0; y <= linesAlongHeight; y++)
	{
		vertices.push_back(-halfSize.x);
		vertices.push_back(-halfSize.y + (float)y * size.y / (float)linesAlongHeight);

		vertices.push_back(halfSize.x);
		vertices.push_back(-halfSize.y + (float)y * size.y / (float)linesAlongHeight);

		indices.push_back(lastIndex + 2 * y);
		indices.push_back(lastIndex + 2 * y + 1);
	}

	vao = VAOFactory::Produce(vertices, indices,
		{
			{ 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0 }
		}
	);

	shader = ShaderManager::GetInstance().Get(GRID_ID);
	if (shader == nullptr)
	{
		shader = ShaderFactory::Produce(
			R"(
				#version 460 core

				layout (location = 0) in vec2 aPos;

				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 perspective;

				void main()
				{
					gl_Position = perspective * view * model * vec4(aPos, 0.0f, 1.0f);
				}
			)",
			R"(
				#version 460 core

				out vec4 FragColor;

				uniform vec4 gridColor;

				void main()
				{
					FragColor = gridColor;
				}
			)"
		);

		if (!shader->Good())
		{
			throw std::runtime_error("Shader creation failed");
		}

		ShaderManager::GetInstance().Register(GRID_ID, shader);
	}

	type = PrimitiveType::Lines;
}

void Grid::InitializeShader(const CameraBase& camera) const
{
	if(glm::dot(glm::rotate(camera.GetQuaternion(), glm::vec3(0.0f, 0.0f, 1.0f)), glm::rotate(GetQuaternion(), glm::vec3(0.0f, 0.0f, 1.0f))) > 0)
		shader->SetUniform("gridColor", glm::vec4(0.5f));
	else
		shader->SetUniform("gridColor", glm::vec4(0.0f));

	shader->SetUniform("model", transformation);
	shader->SetUniform("view", camera.GetView());
	shader->SetUniform("perspective", camera.GetProjection());
}
