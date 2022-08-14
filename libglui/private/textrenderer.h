#pragma once
#include "public/textrenderer.h"
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace glui {
	struct GlyphData {
		types::vec2 pos;
		types::color color;
		types::vec2 size;
		types::vec4 uv;

		GlyphData(types::vec2 pos, types::color color, types::vec2 size, types::vec4 uv) : pos(pos), color(color), size(size), uv(uv) {}
	};

	struct AtlasData {
		types::vec4 uv;
		types::vec2 size;
		int offsetX, offsetY;
		float advanceX;

		AtlasData() :
			uv(0,0,0,0), size(0,0), offsetX(0), offsetY(0), advanceX(0) {}

		AtlasData(types::vec4 uv, types::vec2 size, int offsetX, int offsetY, float advanceX) :
			uv(uv), size(size), offsetX(offsetX), offsetY(offsetY), advanceX(advanceX) {}
	};

	class TextRenderer : public ITextRenderer {
	public:
		TextRenderer(int width, int height);
		~TextRenderer();
		void add(char c, types::vec2 pos, float size, types::color color) override;
		void add(std::string text, types::vec2 pos, float size, types::color color) override;
		void draw() override;
	private:
		void generateAtlas(bool useMipmap = false);

		int _width, _height;

		std::vector<GlyphData> _glyphs;
		std::unordered_map<char, AtlasData> _glyphTextures;

		unsigned int _tex;
		unsigned int _vao, _vbo, _ebo;
		unsigned int _prog;

		// TODO: move this somewhere else?
		FT_Library _ft;

		// TODO: multiple fonts
		FT_Face _face;
	};
}