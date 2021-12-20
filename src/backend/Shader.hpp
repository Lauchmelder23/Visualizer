#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>

class AbstractShader
{
	friend class ShaderFactory;

public:
	AbstractShader(const std::string& vertexShader, const std::string& fragmentShader);
	AbstractShader(const AbstractShader& other) = delete;
	~AbstractShader();

	inline bool Good() { return id != 0; }
	void Use();

	void SetUniform(const std::string& name, const glm::mat4& value);

private:
	unsigned int id;
};

typedef std::shared_ptr<AbstractShader> Shader;

class ShaderFactory
{
public:
	inline static Shader Produce(const std::string& vertexShader, const std::string& fragmentShader)
	{
		return std::make_shared<AbstractShader>(vertexShader, fragmentShader);
	}
};