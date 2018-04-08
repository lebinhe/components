#include "components/circular_buffer/circular_buffer.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

TEST(CircularBuffer, EmptyInt) {
    CircularBuffer<int> cb(5);

    EXPECT_EQ(cb.Size(), 0);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_TRUE(cb.Empty());
    EXPECT_TRUE(cb.MaxSize() > 0);
}

TEST(CircularBuffer, PushBackInt) {
    CircularBuffer<int> cb(5);

    cb.PushBack(1);
    EXPECT_EQ(cb.Size(), 1);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_FALSE(cb.Empty());

    cb.PushBack(2);
    EXPECT_EQ(cb.Size(), 2);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_FALSE(cb.Empty());

    cb.PushBack(3);
    cb.PushBack(4);
    cb.PushBack(5);
    EXPECT_EQ(cb.Size(), 5);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_FALSE(cb.Empty());

    cb.PushBack(6);
    EXPECT_EQ(cb.Size(), 5);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_FALSE(cb.Empty());
}

TEST(CircularBuffer, FrontInt) {
    CircularBuffer<int> cb(5);
    for (int i = 1; i <= 6; ++i) {
        cb.PushBack(i);
    }
    EXPECT_EQ(cb.Size(), 5);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_FALSE(cb.Empty());

    cb.PopFront();
    EXPECT_EQ(cb.Size(), 4);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_FALSE(cb.Empty());
    EXPECT_EQ(cb.Front(), 3);

    cb.PopFront();
    EXPECT_EQ(cb.Size(), 3);
    EXPECT_EQ(cb.Front(), 4);

    cb.PopFront();
    EXPECT_EQ(cb.Size(), 2);
    EXPECT_EQ(cb.Front(), 5);

    cb.PopFront();
    EXPECT_EQ(cb.Size(), 1);
    EXPECT_EQ(cb.Front(), 6);

    cb.PopFront();
    EXPECT_EQ(cb.Size(), 0);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_TRUE(cb.Empty());

    cb.PushBack(7);
    EXPECT_EQ(cb.Size(), 1);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_TRUE(!cb.Empty());
    EXPECT_EQ(cb.Front(), 7);

    cb.PushBack(8);
    cb.PushBack(9);
    EXPECT_EQ(cb.Size(), 3);
    EXPECT_FALSE(cb.Empty());
    EXPECT_EQ(cb.Front(), 7);

    cb.Clear();
    EXPECT_EQ(cb.Size(), 0);
    EXPECT_EQ(cb.Capacity(), 5);
    EXPECT_TRUE(cb.Empty());
}

TEST(CircularBuffer, Index) {
    CircularBuffer<int> cb(5);

    #define ASSERT_THROWS(a, b) {            \
        try {                               \
            a;                              \
            FAIL() << "Failed to throw";    \
        } catch (const b&) {                \
        } catch (...) {                     \
            FAIL() << "Throw wrong thing";  \
        }                                   \
    }

    for (size_t n = 0; n < 10; ++n) {
        EXPECT_TRUE(cb.Empty());

        const CircularBuffer<int>& const_cb(cb);

        ASSERT_THROW(cb.At(0), std::out_of_range);
        ASSERT_THROW(cb.At(1), std::out_of_range);
        ASSERT_THROW(const_cb.At(0), std::out_of_range);
        ASSERT_THROW(const_cb.At(1), std::out_of_range);
        
        cb.PushBack(0);
        cb.PushBack(1);
        cb.PushBack(2);
        EXPECT_EQ(cb.At(0), 0); EXPECT_EQ(const_cb.At(0), 0);
        EXPECT_EQ(cb.At(1), 1); EXPECT_EQ(const_cb.At(1), 1);
        EXPECT_EQ(cb.At(2), 2); EXPECT_EQ(const_cb.At(2), 2);
        EXPECT_EQ(cb[0], 0);    EXPECT_EQ(const_cb[0], 0);
        EXPECT_EQ(cb[1], 1);    EXPECT_EQ(const_cb[1], 1);
        EXPECT_EQ(cb[2], 2);    EXPECT_EQ(const_cb[2], 2);

        EXPECT_EQ(cb.Front(), 0);
        cb[0] = 3;
        EXPECT_EQ(cb.Front(), 3);
        EXPECT_EQ(cb.At(0), 3); EXPECT_EQ(const_cb.At(0), 3);
        EXPECT_EQ(cb.At(1), 1); EXPECT_EQ(const_cb.At(1), 1);
        EXPECT_EQ(cb.At(2), 2); EXPECT_EQ(const_cb.At(2), 2);
        EXPECT_EQ(cb[0], 3);    EXPECT_EQ(const_cb[0], 3);
        EXPECT_EQ(cb[1], 1);    EXPECT_EQ(const_cb[1], 1);
        EXPECT_EQ(cb[2], 2);    EXPECT_EQ(const_cb[2], 2);

        cb[1] = 4;
        EXPECT_EQ(cb.At(0), 3); EXPECT_EQ(const_cb.At(0), 3);
        EXPECT_EQ(cb.At(1), 4); EXPECT_EQ(const_cb.At(1), 4);
        EXPECT_EQ(cb.At(2), 2); EXPECT_EQ(const_cb.At(2), 2);
        EXPECT_EQ(cb[0], 3);    EXPECT_EQ(const_cb[0], 3);
        EXPECT_EQ(cb[1], 4);    EXPECT_EQ(const_cb[1], 4);
        EXPECT_EQ(cb[2], 2);    EXPECT_EQ(const_cb[2], 2);

        cb.PopFront();
        EXPECT_EQ(cb.At(0), 4); EXPECT_EQ(const_cb.At(0), 4);
        EXPECT_EQ(cb.At(1), 2); EXPECT_EQ(const_cb.At(1), 2);
        EXPECT_EQ(cb[0], 4);    EXPECT_EQ(const_cb[0], 4);
        EXPECT_EQ(cb[1], 2);    EXPECT_EQ(const_cb[1], 2);

        cb.PopFront();
        EXPECT_EQ(cb.At(0), 2); EXPECT_EQ(const_cb.At(0), 2);
        EXPECT_EQ(cb[0], 2);    EXPECT_EQ(const_cb[0], 2);

        cb.PopFront();
        EXPECT_TRUE(cb.Empty());
        EXPECT_EQ(cb.Size(), 0);
    }
}

TEST(CircularBuffer, Back) {
    CircularBuffer<int> cb(5);

    cb.PushBack(0);
    cb.PushBack(1);
    cb.PushBack(2);

    EXPECT_EQ(cb.Back(), 2);
    EXPECT_EQ(cb.Size(), 3);

    cb.PopFront();
    EXPECT_EQ(cb.Back(), 2);
    EXPECT_EQ(cb.Size(), 2);
}

TEST(CircularBuffer, Iterator) {
    CircularBuffer<int> cb(5);

    cb.PushBack(0);
    cb.PushBack(1);
    cb.PushBack(2);

    CircularBuffer<int>::iterator b = cb.begin();
    CircularBuffer<int>::iterator e = cb.end();
    EXPECT_EQ(*b, 0);
    EXPECT_EQ(*(--e), 2);
    EXPECT_EQ(*(--e), 1);
    EXPECT_EQ(*(e++), 1);
    EXPECT_EQ(*e, 2);

    int i = 0;
    for (auto& x : cb) {
        EXPECT_EQ(x, i++);
    }
}

