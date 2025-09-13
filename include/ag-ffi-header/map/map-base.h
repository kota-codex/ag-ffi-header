#ifndef AK_MAP_BASE_H_
#define AK_MAP_BASE_H_

// Declarations used by all Map types
//
// Open addressed
// Pow_2 grow
// Linear probe
// Robinhood hashing
//

#include "ag-ffi-header/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    uint64_t  int_val; // must be first for safe {0} initialization
    double    dbl_val;
    AgObject* ptr_val;// can be: shared or own
    AgWeak* weak_val;
} AgMapVal;

typedef struct {
    AgObject* key;   // shared. 0 - empty
    AgMapVal  val;
    uint64_t  dist;  // distance from ideal position
} AgMapBucket;

typedef struct {
    AgObject     head;
    AgMapBucket* buckets;
    size_t       capacity; // must be power of 2
    size_t       size;
} AgMap;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // AK_MAP_BASE_H_
