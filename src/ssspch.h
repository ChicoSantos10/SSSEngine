//
// Created by Francisco Santos on 13/07/2023.
//

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cassert>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// TODO: Add support for other platforms
//  - Create a preprocessor directive for each platform
#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#endif