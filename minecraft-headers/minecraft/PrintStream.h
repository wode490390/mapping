#pragma once

#include <string>

struct PrintStream {
	virtual ~PrintStream(void) = 0;
	virtual void print(std::string const& string) const = 0;

};
