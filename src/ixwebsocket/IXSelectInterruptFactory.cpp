/*
 *  IXSelectInterruptFactory.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "ixwebsocket/IXSelectInterruptFactory.h"

#include "ixwebsocket/IXUniquePtr.h"
#if _WIN32
#include "ixwebsocket/IXSelectInterruptEvent.h"
#else
#include "IXSelectInterruptPipe.h"
#endif

namespace ix
{
    SelectInterruptPtr createSelectInterrupt()
    {
#ifdef _WIN32
        return ix::make_unique<SelectInterruptEvent>();
#else
        return ix::make_unique<SelectInterruptPipe>();
#endif
    }
} // namespace ix
