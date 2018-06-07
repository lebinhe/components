#ifndef COMPONENTS_LRU_CACHE_H_
#define COMPONENTS_LRU_CACHE_H_

#include <stdint.h>

#include "components/util/slice.h"

class Cache;

Cache* NewLRUCache(size_t capacity);

class Cache {
public:
    Cache() = default;

    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;

    virtual ~Cache();

    struct Handle { };

    // Insert a mapping from key->value into the cache and assign it
    // the specified charge against the total cache capacity.
    // 
    // The caller must call this->Release(handle) when 
    // the returned mapping is no longer needed.
    virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                           void (*deleter)(const Slice& key, void* value)) = 0;

    // If the cache has no mapping for "key", returns nullptr.
    // Else return a handle that corresponds to the mapping.  
    virtual Handle* Lookup(const Slice& key) = 0;

    // Release a mapping returned by a previous Lookup().
    virtual void Release(Handle* handle) = 0;

    // Return the value encapsulated in a handle returned by a
    // successful Lookup().
    virtual void* Value(Handle* handle) = 0;

    // If the cache contains entry for key, erase it.
    virtual void Erase(const Slice& key) = 0;

    // Return a new numeric id.
    virtual uint64_t NewId() = 0;

    // Remove all cache entries that are not actively in use.
    virtual void Prune() {}

    // Return an estimate of the combined charges of all elements stored in the
    // cache.
    virtual size_t TotalCharge() const = 0;

private:
    void LRU_Remove(Handle* e);
    void LRU_Append(Handle* e);
    void Unref(Handle* e);

    struct Rep;
    Rep* rep_;

};

#endif  //COMPONENTS_LRU_CACHE_H_
