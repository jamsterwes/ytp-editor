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

out float rad;
out vec2 px;
out vec2 osize;
out vec2 origin;
out vec4 color;

void main() {
	px = (pos * 0.5 + 0.5) * resolution;
	rad = borderRadius;
	origin = offset;
	color = icolor;
	osize = size;
	gl_Position = vec4(pos, 0, 1);
}
)";

static const char* fragment_shader_quad = R"(#version 420

in float rad;
in vec2 px;
in vec2 origin;
in vec2 osize;
in vec4 color;

uniform vec2 resolution;

out vec4 out_color;

float sdf(vec2 v, vec2 org, vec2 size) {
	vec2 root = v - org - size / 2;
	float xmax = (size.x / 2) - rad;
	float ymax = (size.y / 2) - rad;

	if (root.x < xmax && root.x > -xmax || root.y < ymax && root.y > -ymax) {
		// Axis-aligned distance
		vec2 Z = abs(root) - size / 2;
		return max(Z.x, Z.y);
	} else {
		// Rounded corners
		vec2 c1 = vec2(xmax, ymax);
		vec2 c2 = vec2(-xmax, ymax);
		vec2 c3 = vec2(xmax, -ymax);
		vec2 c4 = vec2(-xmax, -ymax);

		return min(
			min(distance(root, c1), distance(root, c2)),
			min(distance(root, c3), distance(root, c4))
		) - rad;
	}
}

void main() {
	float strokeWidth = 1;
	float alphaBig = clamp(0.5 - sdf(px, origin - vec2(strokeWidth,strokeWidth), osize + 2*vec2(strokeWidth,strokeWidth)), 0, 1);
	float alpha = clamp(0.5 - sdf(px, origin, osize), 0, 1);
	//vec4 colorStroke = mix(color, vec4(1,1,1,1), 1 - alpha);
	out_color = vec4(color.rgb, color.a * alpha);
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
	glDeleteProgram(_prog);
	glDeleteBuffers(1, &_ibo);
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
	if (_dirtyBit) {
		rebuffer();
		_dirtyBit = false;
	}

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