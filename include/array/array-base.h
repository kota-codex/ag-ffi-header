#ifndef AG_ARRAY_BASE_H_
#define AG_ARRAY_BASE_H_
#include "runtime.h"

typedef struct {
	AgObject head;
	uint64_t items_count;
	void** items;
} AgBaseArray;

// Moves elements within a container
// x-y-z splits array in 4 spans  0-x, x-y, y-z, x-count
// `move` swaps x-y and y-z spans
bool ag_move_container_items(AgBaseArray* c, uint64_t x, uint64_t y, uint64_t z);

#endif // AG_ARRAY_BASE_H_
