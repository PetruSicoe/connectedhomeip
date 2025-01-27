/*
 *
 *    Copyright (c) 2020-2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file implements heap memory allocation APIs for CHIP. These functions are platform
 *      specific and might be C Standard Library heap functions re-direction in most of cases.
 *
 */

#include <core/CHIPConfig.h>
#include <support/CHIPMem.h>
#include <platform/K32W/memconfig.h>

#include <stdlib.h>
#include <cstring>

//nu vrea doar portable.h, anumite chestii din portable.h depind de alte chestii din freertos
#if defined(ESP_PLATFORM)
#include "freertos/FreeRTOS.h"
#else
#include "FreeRTOS.h"
#endif

#ifndef WEAK
#define WEAK	__attribute__ ((weak))
#endif


#ifndef NDEBUG
#include <atomic>
#include <cstdio>
#endif

#if CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
#include <dmalloc.h>
#include <support/SafeInt.h>
#endif // CHIP_CONFIG_MEMORY_DEBUG_DMALLOC

#if CHIP_CONFIG_MEMORY_MGMT_MALLOC

namespace chip {
namespace Platform {

#ifdef NDEBUG

#define VERIFY_INITIALIZED()
#define VERIFY_POINTER(p)

#else

#define VERIFY_INITIALIZED() VerifyInitialized(__func__)

static std::atomic_int memoryInitialized{ 0 };

static void VerifyInitialized(const char * func)
{
    if (!memoryInitialized)
    {
        fprintf(stderr, "ABORT: chip::Platform::%s() called before chip::Platform::MemoryInit()\n", func);
        abort();
    }
}

#define VERIFY_POINTER(p)                                                                                                          \
    do                                                                                                                             \
        if (((p) != nullptr) && (MemoryDebugCheckPointer((p)) == false))                                                           \
        {                                                                                                                          \
            fprintf(stderr, "ABORT: chip::Platform::%s() found corruption on %p\n", __func__, (p));                                \
            abort();                                                                                                               \
        }                                                                                                                          \
    while (0)

#endif

CHIP_ERROR MemoryAllocatorInit(void * buf, size_t bufSize)
{
#ifndef NDEBUG
    if (memoryInitialized++ > 0)
    {
        fprintf(stderr, "ABORT: chip::Platform::MemoryInit() called twice.\n");
        abort();
    }
#endif
    return CHIP_NO_ERROR;
}

void MemoryAllocatorShutdown()
{
#ifndef NDEBUG
    if (--memoryInitialized < 0)
    {
        fprintf(stderr, "ABORT: chip::Platform::MemoryShutdown() called twice.\n");
        abort();
    }
#endif
#if CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
    dmalloc_shutdown();
#endif // CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
}



WEAK void * __wrap_malloc(size_t size)
{
	return malloc(size);
}

WEAK void __wrap_free(void * ptr)
{
	free(ptr);
}

WEAK void * __wrap_calloc(size_t num, size_t size)
{
	return calloc(num, size);
}

WEAK void * __wrap_realloc(void * ptr, size_t new_size)
{
	return realloc(ptr, new_size);
}





void * MemoryAlloc(size_t size)
{
    VERIFY_INITIALIZED();
    return MemoryAlloc(size, false);
}


void * MemoryAlloc(size_t size, bool isLongTermAlloc)
{
    VERIFY_INITIALIZED();
    //return malloc(size);
    return __wrap_malloc(size);
    //return chip::DeviceLayer::__wrap_malloc(size);
}

void * MemoryCalloc(size_t num, size_t size)
{
    VERIFY_INITIALIZED();
    //return calloc(num, size);
    return __wrap_calloc(num, size);
    //return chip::DeviceLayer::__wrap_calloc(num, size);
}

void * MemoryRealloc(void * p, size_t size)
{
    VERIFY_INITIALIZED();
    VERIFY_POINTER(p);
    //return realloc(p, size);
    return __wrap_realloc(p, size);
    //return chip::DeviceLayer::__wrap_realloc(p, size);
}

void MemoryFree(void * p)
{
    VERIFY_INITIALIZED();
    VERIFY_POINTER(p);
    //free(p);
    __wrap_free(p);
    //return chip::DeviceLayer::__wrap_free(p);
}

bool MemoryInternalCheckPointer(const void * p, size_t min_size)
{
#if CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
    return CanCastTo<int>(min_size) && (p != nullptr) &&
        (dmalloc_verify_pnt(__FILE__, __LINE__, __func__, p, 1, static_cast<int>(min_size)) == MALLOC_VERIFY_NOERROR);
#else  // CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
    return (p != nullptr);
#endif // CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
}

} // namespace Platform
} // namespace chip

#endif // CHIP_CONFIG_MEMORY_MGMT_MALLOC
