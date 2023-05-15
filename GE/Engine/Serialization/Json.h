#pragma once

#include "nlohmann/json.hpp"
#include "../Reflection/ReflectionTypeTraits.h"


using Json = nlohmann::json;
DECLARE_REFLECTION_TYPE(Json);
