#pragma once
#include "public/drawable.h"

namespace glui {
	class Quad : public IDrawable {
	public:
		Quad();
		Quad(types::vec2 pos, types::vec2 size, types::color color);
		~Quad();
		void draw() override;
	private:
		types::vec2 _pos, _size;
		unsigned int _vao, _vbo, _ebo;
		unsigned int _prog;
		types::color _color;
	};
}