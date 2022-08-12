#pragma once

namespace glui {
	namespace types {

		struct vec2 {
			float x, y;
			vec2(float x, float y) : x(x), y(y) {}
		};

		struct vec3 {
			float x, y, z;
			vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		};
		
		struct vec4 {
			float x, y, z, w;
			vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		};
		typedef vec4 color;
	}
}