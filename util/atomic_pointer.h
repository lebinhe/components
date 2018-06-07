#ifndef COMPONENTS_UTIL_ATOMIC_POINTER_H_
#define COMPONENTS_UTIL_ATOMIC_POINTER_H_

#include <stdint.h>

#include <atomic>

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#elif defined(_M_IX86) || defined(__i386__) || defined(__i386)
#define ARCH_CPU_X86_FAMILY 1
#endif


#if defined(ARCH_CPU_X86_FAMILY) && defined(__GNUC__)
inline void MemoryBarrier() {
    __asm__ __volatile__("" : : : "memory");
}
#define HAVE_MEMORY_BARRIER
#endif

#if defined(HAVE_MEMORY_BARRIER)
class AtomicPointer {
public:
    AtomicPointer() { }
    explicit AtomicPointer(void* p) : rep_(p) { }
    inline void* NoBarrier_Load() const { return rep_; }
    inline void NoBarrier_Store(void* v) { rep_ = v; }
    inline void* Acquire_Load() const {
        void* result = rep_;
        MemoryBarrier();
        return result;
    }
    inline void Release_Store(void* v) {
        MemoryBarrier();
        rep_ = v;
    }

private:
    void* rep_;
};

// AtomicPointer based on C++11 <atomic>
#else 
class AtomicPointer {
public:
    AtomicPointer() { }
    explicit AtomicPointer(void* v) : rep_(v) { }
    inline void* Acquire_Load() const {
        return rep_.load(std::memory_order_acquire);
    }
    inline void Release_Store(void* v) {
        rep_.store(v, std::memory_order_release);
    }
    inline void* NoBarrier_Load() const {
        return rep_.load(std::memory_order_relaxed);
    }
    inline void NoBarrier_Store(void* v) {
        retun rep_.store(v, std::memory_order_relaxed);
    }

private:
    std::atomic<void*> rep_;
};

#endif

#undef HAVE_MEMORY_BARRIER
#undef ARCH_CPU_X86_FAMILY

#endif // COMPONENTS_UTIL_ATOMIC_POINTER_H_
