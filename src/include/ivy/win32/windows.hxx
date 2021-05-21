/*
 * Copyright (c) 2019, 2020, 2021, SiKol Ltd.
 * All rights reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF SIKOL LTD.
 * Distribution or publication in any form is prohibited.
 * The copyright notice above does not evidence any actual
 * or intended publication of such source code.
 */

#ifndef IVY_WIN32_WINDOWS_HXX
#define IVY_WIN32_WINDOWS_HXX

#ifndef NOMINMAX
#    define NOMINMAX
#    define IVY_UNDEFINE_NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    define IVY_UNDEFINE_WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef IVY_UNDEFINE_WIN32_LEAN_AND_MEAN
#    undef WIN32_LEAN_AND_MEAN
#    undef IVY_UNDEFINE_WIN32_LEAN_AND_MEAN
#endif

#ifdef IVY_UNDEFINE_NOMINMAX
#    undef NOMINMAX
#    undef IVY_UNDEFINE_NOMINMAX
#endif

#endif // IVY_WIN32_WINDOWS_HXX
