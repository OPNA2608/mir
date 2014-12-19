/*
 * Convenience functions to make logging in Mir easy
 * ~~~
 * Copyright © 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Daniel van Vugt <daniel.van.vugt@canonical.com>
 */

#ifndef MIR_LOG_H_
#define MIR_LOG_H_

#include "mir/logging/logger.h"  // for Severity
#include <string>
#include <cstdarg>

namespace mir
{

void logv(logging::Severity sev, const char *component,
          char const* fmt, va_list va);
void log(logging::Severity sev, const char *component,
         char const* fmt, ...);
void log(logging::Severity sev, const char *component,
         std::string const& message);

#ifndef MIR_LOG_COMPONENT
#ifdef MIR_LOG_COMPONENT_FALLBACK
#define MIR_LOG_COMPONENT MIR_LOG_COMPONENT_FALLBACK
#endif
#endif

#ifdef MIR_LOG_COMPONENT
namespace {
// Isolated namespace so that the component string is always correct for
// where it's used.

inline void log_info(std::string const& message)
{
    ::mir::log(::mir::logging::Severity::informational,
               MIR_LOG_COMPONENT, message);
}

// TODO later as required: error, critical, warning, debug

template<typename... Args>
void log_info(char const* fmt, Args... args)
{
    ::mir::log(::mir::logging::Severity::informational,
               MIR_LOG_COMPONENT, fmt, args...);
}

template<typename... Args>
void log_error(char const* fmt, Args... args)
{
    ::mir::log(::mir::logging::Severity::error,
               MIR_LOG_COMPONENT, fmt, args...);
}

} // (nested anonymous) namespace
#endif

} // namespace mir

#endif // MIR_LOG_H_
