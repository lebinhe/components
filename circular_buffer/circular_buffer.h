#ifndef COMPONENTS_CIRCULAR_BUFFER_CIRCULAR_BUFFER_H_
#define COMPONENTS_CIRCULAR_BUFFER_CIRCULAR_BUFFER_H_

#include <cstdint>
#include <memory>
#include <stdexcept>

template<typename T, typename A = std::allocator<T>> 
class CircularBuffer {
public:
    typedef T                                           value_type;
    typedef A                                           allocator_type;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;

    class                                               iterator;

    explicit CircularBuffer(size_type capacity, 
                            const allocator_type& allocator = allocator_type());
    ~CircularBuffer();

    allocator_type GetAllocator() const;

    size_type Size() const;
    size_type MaxSize() const;
    bool      Empty() const;

    size_type Capacity() const;

    reference       Front();
    const_reference Front() const;
    reference       Back();
    const_reference Back() const;
    reference       operator[](size_type n);
    const_reference operator[](size_type n) const;
    reference       At(size_type n);
    const_reference At(size_type n) const;

    iterator begin();
    iterator end();

    void PushBack(const value_type& value);
    void PopFront();
    void Clear();

private:
    const size_type                 capacity_;
    allocator_type                  allocator_;
    pointer                         buffer_;
    pointer                         front_;
    pointer                         back_; // points to next unused item

    typedef CircularBuffer<T>       class_type;

    CircularBuffer(const class_type&);
    class_type& operator=(const class_type&);

    // NOTE: C++ doesn't guarantee that the sort of arithmetic will 
    //       work with pointers outside the actual allocated object.
    //       So, pointer must be changed in actual allocated object.
    pointer Increment(pointer ptr, size_type by) const {
        const size_type index = ptr - buffer_;
        if (index + by >= capacity_) {
            return ptr + (by - capacity_);
        } else {
            return ptr + by;
        }
    }

    pointer Decrement(pointer ptr, size_type by) const {
        const size_type index = ptr - buffer_;
        if (index >= by) {
            return ptr - by;
        } else {
            return ptr + (capacity_ - by);
        }
    }
};


template<typename T, typename A> 
inline CircularBuffer<T, A>::CircularBuffer(
        size_type capacity, 
        const allocator_type& allocator) 
      : capacity_(capacity), 
        allocator_(allocator),
        buffer_(allocator_.allocate(capacity)),
        front_(nullptr),
        back_(buffer_) {
    }

template<typename T, typename A>
inline CircularBuffer<T, A>::~CircularBuffer() {
    Clear();
    allocator_.deallocate(buffer_, capacity_);
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::allocator_type 
CircularBuffer<T, A>::GetAllocator() const {
    return allocator_;
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::size_type 
CircularBuffer<T, A>::Capacity() const { 
    return capacity_; 
}

template<typename T, typename A>
inline bool CircularBuffer<T, A>::Empty() const { 
    return !front_; 
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::size_type 
CircularBuffer<T, A>::Size() const { 
    return !front_ ? 0 
        : (back_ > front_ ? back_ : back_ + capacity_) - front_;
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::size_type 
CircularBuffer<T, A>::MaxSize() const {
    return allocator_.max_size();
}

template<typename T, typename A>
inline void CircularBuffer<T, A>::PushBack(const value_type& value) {
    if (front_ && front_ == back_) {
        allocator_.destroy(back_);
    }
    
    allocator_.construct(back_, value);
    
    value_type* const next = Increment(back_, 1);
    if (!front_) {
        // empty
        front_ = back_;
        back_ = next;
    } else if (front_ == back_) {
        // full
        front_ = back_ = next;
    } else {
        back_ = next;
    }
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::reference 
CircularBuffer<T, A>::Front() {
    return *front_;
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::const_reference 
CircularBuffer<T, A>::Front() const {
    return *front_;
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::reference 
CircularBuffer<T, A>::Back() {
    return *Decrement(back_, 1);
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::const_reference 
CircularBuffer<T, A>::Back() const {
    return *Decrement(back_, 1);
}

template<typename T, typename A>
inline void CircularBuffer<T, A>::PopFront() {
    allocator_.destroy(front_);

    value_type* const next = Increment(front_, 1);
    if (next == back_) {
        front_ = nullptr;
    } else {
        front_ = next;
    }
}

template<typename T, typename A>
inline void CircularBuffer<T, A>::Clear() {
    if (front_) {
        do {
            allocator_.destroy(front_);
            front_ = Increment(front_, 1);
        } while(front_ != back_);
    }
    front_ = nullptr;
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::reference 
CircularBuffer<T, A>::operator[](size_type n) {
    return *Increment(front_, n);
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::const_reference 
CircularBuffer<T, A>::operator[](size_type n) const {
    return *Increment(front_, n);
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::reference 
CircularBuffer<T, A>::At(size_type n) {
    if (n >= Size()) {
        throw std::out_of_range("Parameter out of range");
    }
    return (*this)[n];
}

template<typename T, typename A>
inline typename CircularBuffer<T, A>::const_reference 
CircularBuffer<T, A>::At(size_type n) const {
    if (n >= Size()) {
        throw std::out_of_range("Parameter out of range");
    }
    return (*this)[n];
}

template<typename T, typename A>
class CircularBuffer<T, A>::iterator 
    : public std::iterator<std::random_access_iterator_tag, 
                           value_type, 
                           size_type, 
                           pointer, 
                           reference> {
public:                          
    typedef CircularBuffer<T, A>            parent_type;
    typedef typename parent_type::iterator  self_type;

    iterator(parent_type& parent, size_type index) 
      : parent_(parent),
        index_(index) {
    }

    self_type& operator++() {
        ++index_;
        return *this;
    }

    self_type operator++(int) {
        self_type old(*this);
        operator++();
        return old;
    }

    self_type& operator--() {
        --index_;
        return *this;
    }

    self_type operator--(int) {
        self_type old(*this);
        operator--();
        return old;
    }

    reference operator*() {
        return parent_[index_];
    }

    pointer operator->() {
        return &(parent_[index_]);
    }

    bool operator==(const self_type& other) const {
        return &parent_ == &other.parent_ && index_ == other.index_;
    }

    bool operator!=(const self_type& other) const {
        return !(other == *this);
    }

private:
    parent_type& parent_;
    size_type    index_;
};

template<typename T, typename A>
typename CircularBuffer<T, A>::iterator 
CircularBuffer<T, A>::begin() {
    return iterator(*this, 0);
}

template<typename T, typename A>
typename CircularBuffer<T, A>::iterator 
CircularBuffer<T, A>::end() {
    return iterator(*this, Size());
}

#endif // COMPONENTS_CIRCULAR_BUFFER_CIRCULAR_BUFFER_H_
