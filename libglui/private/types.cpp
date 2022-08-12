#include "public/glui.h"

using namespace glui;

int fromDigit(char digit) {
	if (digit >= '0' && digit <= '9') {
		return digit - '0';
	} else if (digit >= 'A' && digit <= 'F') {
		return 10 + digit - 'A';
	} else if (digit >= 'a' && digit <= 'f') {
		return 10 + digit - 'a';
	} else {
		return 0;
	}
}

float getHex(const char* two) {
	char hi = two[0];
	char lo = two[1];
	return (fromDigit(hi) * 16 + fromDigit(lo)) / 255.0f;
}

GLUI_API types::color color::fromHex(const char* code) {
	if (strlen(code) == 6) {
		return { getHex(code), getHex(code + 2), getHex(code + 4), 1 };
	} else if (strlen(code) == 8) {
		return { getHex(code), getHex(code + 2), getHex(code + 4), getHex(code + 6) };
	} else {
		return { 0, 0, 0, 1 };
	}
}