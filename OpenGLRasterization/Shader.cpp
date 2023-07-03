#include "Utils.h"
#include "Shader.h"

Shader::Shader(const std::string& filePath):m_FilePath(filePath),m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filePath);
	m_RendererID = CreateShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	GLTryCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
	GLTryCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	auto location = GetUniformLocation(name);
	GLTryCall(glUniform4f(location, f0, f1, f2, f3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	if(m_shaderLocationMap.find(name) != m_shaderLocationMap.end()) return m_shaderLocationMap[name];
	GLTryCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
	{
		std::cout<<"warning: uniform [" << name <<"]does not exist" <<std::endl;
	}
	else
	{
		m_shaderLocationMap[name] = location;
	}
	return location;
}

unsigned int Shader::CompileShader(unsigned int shaderType, const std::string& shaderSource)
{
	unsigned int id = glCreateShader(shaderType);
	const char* src = shaderSource.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(sizeof(char) * length);
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile" << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShaderStr, const std::string& fragmentShaderStr)
{
	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderStr);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderStr);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

ShaderProgramSource Shader::ParseShader(const std::string filePath)
{
	std::ifstream stream(filePath);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::None;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::Vertex;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::Fragment;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}
