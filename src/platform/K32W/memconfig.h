#pragma once

#ifndef SRC_PLATFORM_K32W_MEMCONFIG_H_
#define SRC_PLATFORM_K32W_MEMCONFIG_H_


namespace chip {
namespace Platform {

extern void * __wrap_malloc(size_t size);
extern void __wrap_free(void * ptr);
extern void * __wrap_calloc(size_t num, size_t size);
extern void * __wrap_realloc(void * ptr, size_t new_size);
extern void * __wrap_malloc_r(void * REENT, size_t size);
extern void __wrap_free_r(void * REENT, void * ptr);
extern void * __wrap_realloc_r(void * REENT, void * ptr, size_t new_size);


}
}


#endif /* SRC_PLATFORM_K32W_MEMCONFIG_H_ */
