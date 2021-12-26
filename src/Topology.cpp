#include "Topology.hpp"

#include <vector>

#include "Util.hpp"

Topology::Topology(const glm::vec2& size, const glm::uvec2& subdivisions) :
	texture(nullptr)
{
	// Create VAO
	vao = std::make_shared<lol::VertexArray>();
	
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int y = 0; y < subdivisions.y; y++)
	{
		float yCoord = Map(glm::vec2(0, subdivisions.y), 0.5f * size.y * glm::vec2(-1.0f, 1.0f), y);
		for (unsigned int x = 0; x < subdivisions.x; x++)
		{
			float xCoord = Map(glm::vec2(0, subdivisions.x), 0.5f * size.x * glm::vec2(-1.0f, 1.0f), x);

			vertices.push_back(xCoord);
			vertices.push_back(yCoord);
			vertices.push_back(Map(glm::vec2(-1, subdivisions.x + 1), glm::vec2(0.0f, 1.0f), x));
			vertices.push_back(Map(glm::vec2(-1, subdivisions.y + 1), glm::vec2(0.0f, 1.0f), y));

			if (y > 0 && x > 0)
			{
				indices.push_back(y * subdivisions.x + x);
				indices.push_back(y * subdivisions.x + (x - 1));
				indices.push_back((y - 1) * subdivisions.x + (x - 1));

				indices.push_back(y * subdivisions.x + x);
				indices.push_back((y - 1)* subdivisions.x + (x - 1));
				indices.push_back((y - 1)* subdivisions.x + x);
			}
		}
	}

	std::shared_ptr<lol::VertexBuffer> vertexBuffer = std::make_shared<lol::VertexBuffer>(vertices.size(), vertices);
	vertexBuffer->SetLayout(
		{
			{ lol::Type::Float, 2, false },
			{ lol::Type::Float, 2, false }
		}
	);

	std::shared_ptr<lol::ElementBuffer> elementBuffer = std::make_shared<lol::ElementBuffer>(indices.size(), indices);

	vao->SetVertexBuffer(vertexBuffer);
	vao->SetElementBuffer(elementBuffer);

	// Set up shader
	shader = lol::ShaderManager::GetInstance().Get(TOPOLOGY_ID);
	if (shader == nullptr)
	{
		shader = std::make_shared<lol::Shader>(
			R"(
				#version 460 core

				layout (location = 0) in vec2 position;
				layout (location = 1) in vec2 texCoord;

				out float height;

				uniform mat4 view;
				uniform mat4 projection;
				uniform float offset;
				uniform float heightFactor;

				uniform sampler2D heightmap;

				void main()
				{
					height = texture(heightmap, vec2(texCoord.x + offset, texCoord.y)).x;
					gl_Position = projection * view * vec4(position.x, heightFactor * height, position.y, 1.0f);
				}
			)",
			R"(
				#version 460 core

				in float height;

				out vec4 FragColor;

				void main()
				{
					FragColor = vec4(1.0f - height, 0.0f, height, 1.0f);
				}
			)"
		);

		lol::ShaderManager::GetInstance().Register(TOPOLOGY_ID, shader);
	}

	// Generate image
	image = lol::Image(subdivisions.x, subdivisions.y, lol::PixelFormat::R, lol::PixelType::Float);
}

Topology::~Topology()
{
	if (texture != nullptr)
		delete texture;
}

void Topology::PreRender(const lol::CameraBase& camera) 
{
	if(texture != nullptr)
		texture->Bind();

	shader->SetUniform("view", camera.GetView());
	shader->SetUniform("projection", camera.GetProjection());
	shader->SetUniform("offset", offset);

	shader->SetUniform("heightFactor", heightFactor);
	shader->SetUniform("colorFactor", colorFactor);

	offset += 0.01f;
}

void Topology::MakeTexture()
{
	if (texture != nullptr)
		delete texture;

	texture = new lol::Texture(image, lol::TextureFormat::R32F);
}
