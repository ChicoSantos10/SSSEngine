//
// Created by Francisco Santos on 13/07/2023.
//

#pragma once

// TODO: Rethink about what should actually be here
// NOTE: Do not include platform specific includes since those will propagate throughout the code increasing the chance
//  of mistakes happening. They will be caught at runtime but its cleaner to not be able to do them in the first place

// std includes
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// SSSEngine includes
#include "Types.h"
#include "Assertion.h"
