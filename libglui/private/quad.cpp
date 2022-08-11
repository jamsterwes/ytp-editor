#include "quad.h"
#include <gl/glew.h>

using namespace glui;

const float vdata[8] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
};

const unsigned short idata[6] = {
	0,1,2,
	1,3,2
};

// TODO: move this
static const char* vertex_shader_quad = R"(#version 420

layout (location = 0) in vec2 pos;

uniform vec2 offset;
uniform vec2 size;

void main() {
    gl_Position = vec4(pos * size + offset, 0.0, 1.0);
}
)";

static const char* fragment_shader_quad = R"(#version 420

out vec4 out_color;

uniform vec4 color;

void main() {
    out_color = color;
}
)";

unsigned int loadShader(GLenum type, const char* source) {
	unsigned int shader = glCreateShader(type);
	size_t len = strlen(source);
	glShaderSource(shader, 1, &source, (const int*)&len);
	glCompileShader(shader);
	return shader;
}

unsigned int loadProgram(const char* vertexSource, const char* fragmentSource) {
	unsigned int vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}

Quad::Quad() : Quad({ 0,0 }, { 0,0 }, {0,0,0,1}) {}

Quad::Quad(types::vec2 pos, types::vec2 size, types::color color) : _pos(pos), _size(size), _vbo(0), _prog(0), _color(color) {
	// Generate VAO (MOVE THIS)
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate VBO,EBO (TODO: reuse VBO,EBO)
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	// Initialize vertex format (MOVE THIS)
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Buffer quad data
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vdata, GL_STATIC_DRAW);

	// Buffer index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6, idata, GL_STATIC_DRAW);

	// Generate program
	_prog = loadProgram(vertex_shader_quad, fragment_shader_quad);
}

Quad::~Quad() {
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

void Quad::draw() {
	// Bind VAO
	glBindVertexArray(_vao);
	
	// Bind VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	// Bind shader
	glUseProgram(_prog);

	// Set uniforms
	glUniform2f(glGetUniformLocation(_prog, "offset"), _pos.x, _pos.y);
	glUniform2f(glGetUniformLocation(_prog, "size"), _size.x, _size.y);
	glUniform4f(glGetUniformLocation(_prog, "color"), _color.x, _color.y, _color.z, _color.w);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}