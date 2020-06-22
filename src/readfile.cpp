#include "readfile.h"

#include <limits>
#include <fstream>

std::string readfile(const std::string_view path)
{
	std::ifstream ifs(path.data(), std::ios::in | std::ios::binary);
	ifs.ignore(std::numeric_limits<std::streamsize>::max());
	std::string data(ifs.gcount(), 0);
	ifs.clear();
	ifs.seekg(0, std::ios_base::beg);
    ifs.read(data.data(), data.size());
    return data;
}