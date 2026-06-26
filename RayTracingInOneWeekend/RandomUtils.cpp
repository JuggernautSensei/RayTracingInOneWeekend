#include "pch.h"
#include "RandomUtils.h"

namespace Details
{

thread_local RandomGeneratorSHR3 g_rngSHR3 { kRandomDeviceSeed };

}