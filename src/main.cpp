#include <iostream>

#include "miklua.h"
using namespace mik;

int main(int argc, char** argv) {
	MikLua::run("../res/test.lua");
	return 0;
}