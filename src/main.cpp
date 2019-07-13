#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <cassert>
#include <streambuf>
#include <regex>

#include "argparse.hpp"
#include "byteutils.h"
#include "cartridge.h"

#include "log.h"
#include "miklua.h"
using namespace mik;

void trim(std::string& str) {
	while (str[0] == '"' || str[0] == '\'' || ::isspace(str[0])) {
		str.erase(str.begin());
	}
	while (str.back() == '"' || str.back() == '\'' || ::isspace(str.back())) {
		str.pop_back();
	}
}

std::vector<std::string> extractStrings(std::string const& str) {
	const std::regex r("[\"'](.*?)[\"']");
	std::smatch sm;
	std::vector<std::string> ret;

	std::string strr(str);
	while (std::regex_search(strr, sm, r)) {
		std::string match = sm[0];
		trim(match);
		std::string ext = match.substr(match.find_last_of('.')+1);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (ext == "wav" || ext == "png") {
			ret.push_back(match);
		}
		strr = sm.suffix();
	}

	return ret;
}

int main(int argc, char** argv) {
	ArgumentParser parser("Mikro Platform v1.0");
	parser.add_argument("--dist", "Distribute Game");

	try {
		parser.parse(argc, argv);
	} catch (const ArgumentParser::ArgumentNotFound& ex) {
		LogE(ex.what());
		return 1;
	}
	if (parser.is_help()) return 0;

	auto freeArgs = parser.getv<std::string>("");
	if (freeArgs.empty()) {
		LogE("No cartridge specified.");
	} else {
		if (parser.get<bool>("dist")) {
			std::string script = freeArgs[0];
			std::replace(script.begin(), script.end(), '\\', '/');

			u32 n = script.find_last_of('.');
			const std::string name = script.substr(0, n);

			MikLua::init();
			auto code = MikLua::compile(script);
			MikLua::deinit();

			// Extract the strings used in the script
			std::vector<std::string> filesInScript;
			std::ifstream sfp(script);
			if (sfp.good()) {
				LogI("Retrieving files...");
				std::string str((std::istreambuf_iterator<char>(sfp)),
									std::istreambuf_iterator<char>());
				filesInScript = extractStrings(str);
				for (auto&& s : filesInScript)
					LogI(s);
			} else {
				LogE("Invalid script file.");
				return 1;
			}

			// Pack the files in the cartridge
			CartridgePtr cart = std::make_unique<Cartridge>();
			cart->script(code);
			for (const std::string& fn : filesInScript) {
				std::ifstream fp(fn, std::ios::binary);
				if (fp.good()) {
					LogI("Packing \"", fn, "\"...");
					fp.unsetf(std::ios::skipws);

					fp.seekg(0, std::ios::end);
					auto sz = fp.tellg();
					fp.seekg(0, std::ios::beg);

					std::vector<u8> data;
					data.reserve(sz);
					data.insert(data.begin(), std::istream_iterator<u8>(fp), std::istream_iterator<u8>());
					fp.close();

					std::string ext = fn.substr(fn.find_last_of('.')+1);
					std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

					FileEntry* fe = new FileEntry();
					fe->size = sz;
					fe->name = fn;
					fe->type = ext;
					fe->data = data;
					cart->files().push_back(std::unique_ptr<FileEntry>(fe));
				}
			}

			// Write the cartridge
			auto dat = cart->save();
			std::ofstream of(name + ".gam", std::ios::binary);
			of.write(reinterpret_cast<const char*>(dat.data()), dat.size());
			of.close();

			LogI("Wrote ", dat.size() / 1024, " KB");
			LogI("Success!");
		} else {
			MikLua::init();
			MikLua::run(freeArgs[0]);
			MikLua::deinit();
		}
	}
	return 0;
}