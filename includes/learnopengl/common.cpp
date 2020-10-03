#include "common.h"

void careOpen(std::ifstream& f, const std::string& fileName) {
	// ensure ifstream objects can throw exceptions:
	//f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f.open(fileName);
	if (!f)
		throw std::system_error(errno, std::system_category(),
			"failed to open " + fileName);
}