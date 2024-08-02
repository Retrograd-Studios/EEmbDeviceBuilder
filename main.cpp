

#include <iostream>

#include <string>

#include "Renderer/Render.h"


std::string SimPath = "./";



int main(int argc, const char* argv[])
{
	SimPath = std::string(argv[0]) + "/../../EEmbSimulator/";

	EEmbSimulator::Render render{};
	auto result = render.run();
	return result;
}