#pragma once
#include "shared_export.h"
#include "observable/subject.hpp"

namespace shared {

SHARED_EXPORT void subscribe_in_shared_lib(observable::subject<void()> & subject);

SHARED_EXPORT void notify_void(observable::subject<void()> & subject);

SHARED_EXPORT int call_count();

SHARED_EXPORT void increment_call_count();

SHARED_EXPORT void reset_call_count();

}
