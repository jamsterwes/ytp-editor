#include "private/textrenderer.h"

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
uniform vec2 offset;
uniform vec4 icolor;
uniform vec2 size;
uniform vec4 uvbounds;

uniform vec2 resolution;

out vec2 uv;
out vec4 color;

void main() {
	color = icolor;
	vec2 fake_uv = (pos * 0.5 + 0.5);
	uv = fake_uv;
	// uv = vec2(mix(uvbounds.x, uvbounds.y, fake_uv.x), mix(uvbounds.z, uvbounds.w, fake_uv.y));
	vec2 position = fake_uv * (2 * size / resolution) + 2 * offset / resolution - vec2(1, 1);
	gl_Position = vec4(position, 0, 1);
}
)";

static const char* fragment_shader_quad = R"(#version 420

in vec2 uv;
in vec4 color;

uniform vec2 resolution;
uniform sampler2D glyphTexture;

out vec4 out_color;

void main() {
	float alpha = texture2D(glyphTexture, uv).r;
	// out_color = vec4(color.rgb, color.a * alpha);
	out_color = vec4(uv,alpha,1);
}
)";

unsigned int loadShader(GLenum type, const char* source);
unsigned int loadProgram(const char* vertexSource, const char* fragmentSource);

TextRenderer::TextRenderer(int width, int height) : _width(width), _height(height), _glyphs(), _glyphTextures() {
	// Generate VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate VBO,EBO
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	// Vertex Format [0]: vec2 pos // VBO
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Vertex Format [1]: vec2 offset // uniform
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, _ibo);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
	//glVertexAttribDivisor(1, 1);

	// Vertex Format [2]: vec4 color // uniform
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(2 * sizeof(float)));
	//glVertexAttribDivisor(2, 1);

	// Vertex Format [3]: vec2 size // uniform
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(6 * sizeof(float)));
	//glVertexAttribDivisor(3, 1);

	// Buffer quad data
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vdata, GL_STATIC_DRAW);

	// Buffer index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6, idata, GL_STATIC_DRAW);

	// Generate program
	_prog = loadProgram(vertex_shader_quad, fragment_shader_quad);

	// Load Freetype
	if (FT_Init_FreeType(&_ft))
	{
		printf("Failed to load FreeType!\n");
	}

	// Load font
	if (FT_New_Face(_ft, "fonts/Roboto-Regular.ttf", 0, &_face))
	{
		printf("Failed to load font!\n");
	}

	// Generate atlas
	generateAtlas();
}

TextRenderer::~TextRenderer() {
	glDeleteProgram(_prog);
	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

void TextRenderer::generateAtlas() {
	int resolution = 2048;
	int xptr = 0;
	int yptr = 0;

	// Generate texture
	glGenTextures(1, &_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, resolution, resolution, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	// Texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	FT_GlyphSlot slot = _face->glyph;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (int c = 32; c < 128; c++) {
		// Render glyph
		FT_Set_Pixel_Sizes(_face, 0, 128);
		if (FT_Load_Char(_face, (char)c, FT_LOAD_RENDER)) {
			printf("Error loading glyph '%c'\n", (char)c);
			return;
		}

		// Determine if need new row?
		if (xptr + slot->bitmap.width > resolution) {
			xptr = 0;
			yptr += 128;  // TODO: padding?
		}

		// Create atlas data
		AtlasData data = AtlasData({ (float)xptr / (float)resolution, (float)yptr / (float)resolution, (xptr + (float)slot->bitmap.width) / (float)resolution, (yptr + (float)slot->bitmap.rows) / (float)resolution },
			{ (float)slot->bitmap.width, (float)slot->bitmap.rows },
			slot->bitmap_left, slot->bitmap_top,
			slot->advance.x / 64.f
		);
		_glyphTextures.insert({ c, data });

		printf("Rendering %c at (%d, %d)\n", (char)c, xptr, yptr);
		printf("UV (%.2f, %.2f, %.2f, %.2f)\n", (float)xptr / (float)resolution, (float)yptr / (float)resolution, (xptr + (float)slot->bitmap.width) / (float)resolution, (yptr + (float)slot->bitmap.rows) / (float)resolution);

		// Upload glyph
		glTexSubImage2D(GL_TEXTURE_2D, 0, xptr, yptr, slot->bitmap.width, slot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);

		// Advance ptrs
		xptr += slot->bitmap.width + slot->advance.x / 64.f + 1;
		if (xptr >= resolution) {
			xptr = 0;
			yptr += 128;
		}
	}
}

void TextRenderer::add(char c, types::vec2 pos, float size, types::color color) {
	add(std::string(1, c), pos, size, color);
}

void TextRenderer::add(std::string text, types::vec2 pos, float size, types::color color) {
	int advanceX = 0;
	float delta = size / 128;
	for (int i = 0; i < text.size(); i++) {
		AtlasData& data = _glyphTextures[text[i]];
		_glyphs.push_back({
			{pos.x + advanceX + data.offsetX * delta, pos.y + data.size.y * delta - data.offsetY * delta},
			color,
			{data.size.x * delta, data.size.y * delta},
			data.uv
		});
		advanceX += data.advanceX * delta;
	}
}

void TextRenderer::draw() {
	// Bind VAO
	glBindVertexArray(_vao);

	// Bind shader
	glUseProgram(_prog);

	// Resolution uniform
	glUniform2f(glGetUniformLocation(_prog, "resolution"), _width, _height);
	glUniform1i(glGetUniformLocation(_prog, "glyphTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _tex);

	// Draw
	for (int i = 0; i < _glyphs.size(); i++) {
		//printf("Drawing glyph @ (%d, %d)\n", (int)_glyphs[i].pos.x, (int)_glyphs[i].pos.y);
		//printf("        UV @ (%.2f, %.2f) to (%.2f, %.2f)\n", _glyphs[i].uv.x, _glyphs[i].uv.y, _glyphs[i].uv.z, _glyphs[i].uv.w);

		glUniform2f(glGetUniformLocation(_prog, "offset"), _glyphs[i].pos.x, _glyphs[i].pos.y);
		glUniform4f(glGetUniformLocation(_prog, "icolor"), _glyphs[i].color.x, _glyphs[i].color.y, _glyphs[i].color.z, _glyphs[i].color.w);
		glUniform2f(glGetUniformLocation(_prog, "size"), _glyphs[i].size.x, _glyphs[i].size.y);
		glUniform4f(glGetUniformLocation(_prog, "uvbounds"), _glyphs[i].uv.x, _glyphs[i].uv.y, _glyphs[i].uv.z, _glyphs[i].uv.w);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}
}