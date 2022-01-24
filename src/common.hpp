#pragma once
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <csv.h> // vendor/csv-parser
#include <nlohmann/json.hpp> // vendor/json/single_include
#include <ta-lib/ta_libc.h>

namespace program
{
    using namespace std::chrono_literals;
}