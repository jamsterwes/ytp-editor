#include "quadrenderer.h"
#include <gl/glew.h>

using namespace glui;

const float vdata[8] = {
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f, 1.0f,
	1.0f, 1.0f
};

const unsigned short idata[6] = {
	0,1,2,
	1,3,2
};

// TODO: move this
static const char* vertex_shader_quad = R"(#version 420

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 offset;
layout (location = 2) in vec4 icolor;
layout (location = 3) in vec2 size;
layout (location = 4) in float borderRadius;

uniform vec2 resolution;

out vec2 cornerCut;
out vec2 uv;
out vec4 color;

void main() {
	cornerCut = vec2(2 * borderRadius / (size.x), 2 * borderRadius / (size.y));
	uv = pos;
	color = icolor;
    gl_Position = vec4(((pos + 1) / 2) * vec2(2 * size.x / resolution.x, 2 * size.y / resolution.y) + vec2(2 * offset.x / resolution.x, 2 * offset.y / resolution.y) - vec2(1, 1), 0.0, 1.0);
}
)";

static const char* fragment_shader_quad = R"(#version 420

in vec2 cornerCut;
in vec2 uv;
in vec4 color;

out vec4 out_color;

float cornerNorm(vec2 _uv, vec2 _cornerCut) {
	vec2 norm = _uv - 1 + _cornerCut;
	norm = vec2(norm.x / _cornerCut.x, norm.y / _cornerCut.y);
	return distance(vec2(0,0), norm);
}

float rescale(vec2 start, vec2 end, float t) {
	float n = (t - start.x) / (start.y - start.x);
	return end.x + smoothstep(0, 1, n) * (end.y - end.x);
}

float fakeAA(float t) {
	return clamp(rescale(vec2(1, 1 + 0.0125), vec2(1, 0), t), 0, 1);
}

void main() {
	float alpha = 1;
	if (uv.x > (1 - cornerCut.x) && uv.y > (1 - cornerCut.y)) {
		alpha = fakeAA(cornerNorm(uv, cornerCut));
	} else if (uv.x < -(1 - cornerCut.x) && uv.y > (1 - cornerCut.y)) {
		alpha = fakeAA(cornerNorm(uv * vec2(-1,1), cornerCut));
	} else if (uv.x < -(1 - cornerCut.x) && uv.y < -(1 - cornerCut.y)) {
		alpha = fakeAA(cornerNorm(uv * vec2(-1,-1), cornerCut));
	} else if (uv.x > (1 - cornerCut.x) && uv.y < -(1 - cornerCut.y)) {
		alpha = fakeAA(cornerNorm(uv * vec2(1,-1), cornerCut));
	}
	out_color = vec4(color.rgb, alpha * color.a);
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

QuadRenderer::QuadRenderer(int width, int height) : _width(width), _height(height), _vbo(0), _prog(0), _dirtyBit(false) {
	// Generate VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate VBO,EBO
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);
	glGenBuffers(1, &_ibo);

	// Vertex Format [0]: vec2 pos // VBO
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Vertex Format [1]: vec2 offset // IBO
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _ibo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
	glVertexAttribDivisor(1, 1);

	// Vertex Format [2]: vec4 color // IBO
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(2 * sizeof(float)));
	glVertexAttribDivisor(2, 1);

	// Vertex Format [3]: vec2 size // IBO
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(6 * sizeof(float)));
	glVertexAttribDivisor(3, 1);

	// Vertex Format [4]: float borderRadius // IBO
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(8 * sizeof(float)));
	glVertexAttribDivisor(4, 1);

	// Buffer quad data
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vdata, GL_STATIC_DRAW);

	// Buffer index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6, idata, GL_STATIC_DRAW);

	// Generate program
	_prog = loadProgram(vertex_shader_quad, fragment_shader_quad);
}

QuadRenderer::~QuadRenderer() {
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

void QuadRenderer::draw() {
	// Bind VAO
	glBindVertexArray(_vao);

	// Bind shader
	glUseProgram(_prog);

	// Resolution uniform
	glUniform2f(glGetUniformLocation(_prog, "resolution"), _width, _height);

	// Rebuffer
	if (_dirtyBit) rebuffer();

	// Draw
	if (_quads.size() > 0) {
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, _quads.size());
	}
}

void QuadRenderer::add(types::vec2 pos, types::vec2 size, types::color color, float borderRadius) {
	// Add new quad
	_quads.push_back({ pos, size, color, borderRadius });
	
	// Set dirty bit
	_dirtyBit = true;
}

void QuadRenderer::rebuffer() {
	// Rebuffer data
	glBindBuffer(GL_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad) * _quads.size(), _quads.data(), GL_STATIC_DRAW);
}