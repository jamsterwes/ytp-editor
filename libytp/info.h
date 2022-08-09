#pragma once
#include "pch.h"

// TODO: fix dllexport/dllimport

__declspec(dllexport) int getVersionMinor() {
	return VERSION_MINOR;
}

__declspec(dllexport) int getVersionMajor() {
	return VERSION_MAJOR;
}