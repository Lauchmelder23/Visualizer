#include "Cuboid.hpp"

#include <stdexcept>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "backend/ObjectManager.hpp"
#include "backend/Camera.hpp"
#include "Util.hpp"

Cuboid::Cuboid()
{
	vao = VAOManager::GetInstance().Get(CUBOID_ID);
	if (vao == nullptr)
	{
		vao = VAOFactory::Produce(
			{
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f
			},
			{
				0, 1, 1, 2, 2, 3, 3, 0, // Front
				4, 5, 5, 6, 6, 7, 7, 4, // Back
				0, 4, 1, 5, 2, 6, 3, 7
			},
			{
				{ 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 }
			}
		);

		VAOManager::GetInstance().Register(CUBOID_ID, vao);
	}

	shader = ShaderManager::GetInstance().Get(CUBOID_ID);
	if (shader == nullptr)
	{
		shader = ShaderFactory::Produce(
			R"(
				#version 460 core

				layout (location = 0) in vec3 aPos;

				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 perspective;

				void main()
				{
					gl_Position = perspective * view * model * vec4(aPos, 1.0f);
				}
			)",
			R"(
				#version 460 core

				out vec4 FragColor;

				void main()
				{
					FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				}
			)"
		);

		if (!shader->Good())
		{
			throw std::runtime_error("Shader creation failed");
		}

		ShaderManager::GetInstance().Register(CUBOID_ID, shader);
	}

	type = PrimitiveType::Lines;
}

void Cuboid::InitializeShader(const CameraBase& camera) const
{
	shader->SetUniform("model", transformation);
	shader->SetUniform("view", camera.GetView());
	shader->SetUniform("perspective", camera.GetProjection());
}
