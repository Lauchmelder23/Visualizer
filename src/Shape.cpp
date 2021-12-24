#include "Shapes.hpp"

#include "Util.hpp"

Shape::Shape(const std::shared_ptr<lol::Texture>& texture)
{
	shader = lol::ShaderManager::GetInstance().Get(SHAPE_ID);
	if (shader == nullptr)
	{
		shader = std::make_shared<lol::Shader>(
			R"(
				#version 460 core
	
				layout (location = 0) in vec3 pos;
				layout (location = 1) in vec2 uv;				

				out vec2 UVcoord;

				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;

				void main()
				{
					UVcoord = uv;
					gl_Position = projection * view * model * vec4(pos, 1.0f);
				}
			)",
			R"(
				#version 460 core
	
				out vec4 FragColor;

				in vec2 UVcoord;

				uniform sampler2D shapeTexture;
				
				void main()
				{
					FragColor = texture(shapeTexture, UVcoord);
				}
			)"
			);

		lol::ShaderManager::GetInstance().Register(SHAPE_ID, shader);
	}

	this->texture = texture;
}

Shape::~Shape()
{
	lol::ShaderManager::GetInstance().Return(SHAPE_ID);
}

void Shape::PreRender(const lol::CameraBase& camera) const
{
	texture->Bind();

	shader->SetUniform("model", transformation);
	shader->SetUniform("view", camera.GetView());
	shader->SetUniform("projection", camera.GetProjection());
}

Cube::Cube(const std::shared_ptr<lol::Texture>& texture) :
	Shape(texture)
{
	vao = lol::VAOManager::GetInstance().Get(CUBE_ID);
	if (vao == nullptr)
	{
		std::shared_ptr<lol::VertexBuffer> vbo = std::make_shared<lol::VertexBuffer>(8 * (3 + 2),
			std::vector<float> {
				-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,
				 1.0f, -1.0f, -1.0f,	1.0f, 0.0f,
				 1.0f,  1.0f, -1.0f,	1.0f, 1.0f,
				-1.0f,  1.0f, -1.0f,	0.0f, 1.0f,
				-1.0f, -1.0f,  1.0f,	0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f,	1.0f, 0.0f,
				 1.0f,  1.0f,  1.0f,	1.0f, 1.0f,
				-1.0f,  1.0f,  1.0f,	0.0f, 1.0f,
			}
		);
		vbo->SetLayout(
			{
				{lol::Type::Float, 3, false},
				{lol::Type::Float, 2, false}
			}
		);

		std::shared_ptr<lol::ElementBuffer> ebo = std::make_shared<lol::ElementBuffer>(6 * 3 * 2,
			std::vector<unsigned int> {
				0, 1, 3, 3, 1, 2,
				1, 5, 2, 2, 5, 6,
				5, 4, 6, 6, 4, 7,
				4, 0, 7, 7, 0, 3,
				3, 2, 7, 7, 2, 6,
				4, 5, 0, 0, 5, 1
			}
		);

		vao = std::make_shared<lol::VertexArray>(vbo, ebo);
		lol::VAOManager::GetInstance().Register(CUBE_ID, vao);
	}
}

Cube::~Cube()
{
	lol::VAOManager::GetInstance().Return(CUBE_ID);
}

Pyramid::Pyramid(const std::shared_ptr<lol::Texture>& texture) :
	Shape(texture)
{
	vao = lol::VAOManager::GetInstance().Get(PYRAMID_ID);
	if (vao == nullptr)
	{
		std::shared_ptr<lol::VertexBuffer> vbo = std::make_shared<lol::VertexBuffer>(5 * (3 + 2),
			std::vector<float> {
				-1.0f, -0.86f, 1.0f,		0.0f, 0.0f,	
				1.0f, -0.86f, 1.0f,			1.0f, 0.0f,
				-1.0f, -0.86f, -1.0f,		0.0f, 1.0f,
				1.0f, -0.86f, -1.0f,		1.0f, 0.0f,
				0.0f, 0.86f, 0.0f,			0.5f, 1.0f
		}
		);
		vbo->SetLayout(
			{
				{lol::Type::Float, 3, false},
				{lol::Type::Float, 2, false}
			}
		);

		std::shared_ptr<lol::ElementBuffer> ebo = std::make_shared<lol::ElementBuffer>(18,
			std::vector<unsigned int> {
				0, 2, 1,
				1, 2, 3,
				0, 1, 4,
				1, 3, 4,
				3, 2, 4,
				2, 0, 4,
		}
		);

		vao = std::make_shared<lol::VertexArray>(vbo, ebo);
		lol::VAOManager::GetInstance().Register(PYRAMID_ID, vao);
	}
}

Pyramid::~Pyramid()
{
	lol::VAOManager::GetInstance().Return(PYRAMID_ID);
}
