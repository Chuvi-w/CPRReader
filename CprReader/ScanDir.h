#pragma once
#include <FSHeader.h>
#include <vector>
#include <map>



std::map<fs::path, std::vector<fs::path>> ListDirectory(const fs::path &dir);
