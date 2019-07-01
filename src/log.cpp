#include "log.h"
#include <cstdarg>
#include <ctime>

void Log::log(
	LogLevel level,
	const char* file,
	const char* function,
	int line,
	const std::string& msg
) {

#define L_OUT std::cout

#define BOLD(o, msg, col) o << col << termcolor::bold << msg << termcolor::reset
#define NORM(o, msg, col) o << col << msg << termcolor::reset

	time_t now = time(0);
	struct tm tstruct;
	char timeBuf[80] = {0};
	tstruct = *localtime(&now);
	strftime(timeBuf, sizeof(timeBuf), "%m/%d/%Y %X", &tstruct);

	L_OUT << "[";
	NORM(L_OUT, timeBuf, termcolor::green);
	L_OUT << "] ";

	switch (level) {
		case Info: BOLD(L_OUT, "[INFO]", termcolor::cyan); break;
		case Warning: BOLD(L_OUT, "[WARN]", termcolor::yellow); break;
		case Error: BOLD(L_OUT, "[ERROR]", termcolor::red); break;
		case Assert: BOLD(L_OUT, "[FAIL]", termcolor::magenta << termcolor::blink); break;
	}

	std::string fileS(file);
	fileS = fileS.substr(fileS.find_last_of(SEP)+1);

	L_OUT << " [" << fileS << "(" << function << ")@" << line << "] " << msg << std::endl;

#undef L_OUT
}