#pragma once
#include "public/quadrenderer.h"

namespace glui {
	#pragma pack(push, 1)
	struct Quad {
		types::vec2 pos;
		types::color color;
		types::vec2 size;
		float borderRadius;

		Quad(types::vec2 pos, types::vec2 size, types::color color, float borderRadius = 0.0f) : pos(pos), size(size), color(color), borderRadius(borderRadius) {}
	};
	#pragma pack(pop)

	class QuadRenderer : public IQuadRenderer {
	public:
		QuadRenderer(int width, int height);
		~QuadRenderer();
		void add(types::vec2 pos, types::vec2 size, types::color color, float borderRadius) override;
		void draw() override;
	private:
		void rebuffer();

		int _width, _height;

		std::vector<Quad> _quads;
		bool _dirtyBit;

		unsigned int _vao, _vbo, _ebo, _ibo;
		unsigned int _prog;
	};
}