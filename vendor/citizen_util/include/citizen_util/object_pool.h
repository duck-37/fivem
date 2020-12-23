#pragma once
#include <algorithm>
#include <atomic>
#include <deque>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>

#include "detached_queue.h"
#include "type_helpers.h"

#include <xenium/ramalhete_queue.hpp>
#include <xenium/reclamation/generic_epoch_based.hpp>

namespace fx {
// Object pools allow for fast singular type allocation based on plf::colony.
//
template<typename T, size_t NumEntries = 2048, size_t GrowthCap = 5, size_t GrowthFactor = 1>
struct object_pool {
    // Forward declares and type exports.
    //
    using value_type = T;
    struct pool_instance;
    struct bucket_entry;

	static constexpr size_t Align = alignof(T);

    // Queue of free memory regions.
    //
	inline static xenium::ramalhete_queue<
		T*,
		xenium::policy::reclaimer<xenium::reclamation::generic_epoch_based<>>,
		xenium::policy::entries_per_node<2048>
	> free_queue;

	// Mutex for growth.
	//
	inline static std::mutex growth_mutex;

	// Current "growth stage" of the pool.
	//
	inline static size_t current_growth_stage;

	object_pool() = default;

    // Delete copy/move constructors.
    //
    object_pool(const object_pool &other) = delete;

    object_pool(object_pool &&other) = delete;

    object_pool &operator=(const object_pool &other) = delete;

    object_pool &operator=(object_pool &&other) = delete;

	static T* allocate()
	{
		T* key;
		if (free_queue.try_pop(key))
		{
			// Return the entry.
			//
			return key;
		}

		std::scoped_lock free_lock(growth_mutex);

		// Try again with lock.
		if (free_queue.try_pop(key))
		{
			// Return the entry.
			//
			return key;
		}

		// Allocate a new pool.
		//
		size_t growth_stage = std::min<size_t>(current_growth_stage, GrowthCap);
		++current_growth_stage;

		// Determine new pool's size (raw size is merely an approximation).
		//
		size_t object_count = NumEntries << (growth_stage * GrowthFactor);

		// Allocate the pool, keep the first object to ourselves.
		//
		auto* new_objects = (T*)_aligned_malloc(sizeof(T) * object_count, Align);

		// Initialize every other object.
		//
		for (size_t i = 1; i < object_count - 1; i++)
			free_queue.push(&new_objects[i]);

		// Return the first allocated address.
		//
		return new_objects;
	}

    // Construct / destruct wrappers.
    //
    template<typename... Tx>
    static T *construct(Tx &&... args) {
        return new(allocate()) T(std::forward<Tx>(args)...);
    }

    static void destruct(T *pointer) {
        std::destroy_at<T>(pointer);
        free_queue.push(pointer);
    }
};
}