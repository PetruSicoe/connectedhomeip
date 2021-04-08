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

#include "FreeRTOS.h"
#include <cstring>
#include <stdlib.h>
#include <malloc.h>

#ifndef NDEBUG
#include <atomic>
#include <cstdio>
#endif

#if CHIP_CONFIG_MEMORY_DEBUG_DMALLOC
#include <dmalloc.h>
#include <support/SafeInt.h>
#endif // CHIP_CONFIG_MEMORY_DEBUG_DMALLOC

namespace chip {
namespace Platform {

extern "C" {

void * __wrap_malloc(size_t size)
{
    return pvPortMalloc(size);
}

void __wrap_free(void * ptr)
{
    vPortFree(ptr);
}

void * __wrap_calloc(size_t num, size_t size)
{
    size_t total_size = num * size;

    if (size && total_size / size != num)
        return nullptr;

    void * ptr = pvPortMalloc(total_size);
    if (ptr)
        memset(ptr, 0, total_size);
    return ptr;
}

void * __wrap_realloc(void * ptr, size_t new_size)
{
    if (new_size)
    {
        void * new_ptr = pvPortMalloc(new_size);
        if (new_ptr)
        {
            if (ptr)
            {
                memset(new_ptr, 0, new_size);
                memcpy(new_ptr, ptr, malloc_usable_size(ptr));
                vPortFree(ptr);
            }
            return new_ptr;
        }
    }
    else
    {
        vPortFree(ptr);
    }

    return NULL;
}

void * __wrap__malloc_r(void * REENT, size_t size)
{
    return __wrap_malloc(size);
}

void __wrap__free_r(void * REENT, void * ptr)
{
    __wrap_free(ptr);
}

void * __wrap__realloc_r(void * REENT, void * ptr, size_t new_size)
{
    return __wrap_realloc(ptr, new_size);
}

} // extern "C"

} // namespace Platform
} // namespace chip
