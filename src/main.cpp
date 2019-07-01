#include <iostream>

#include "log.h"
#include "miklua.h"
using namespace mik;

int main(int argc, char** argv) {
	if (argc < 2) {
		LogE("No script specified.");
	} else {
		MikLua::run(std::string(argv[1]));
	}
	return 0;
}