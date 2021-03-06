#pragma once
#include <cstdlib>
#include <utility>
#include <exception>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <stdint.h>
#include <cassert>

namespace {
    constexpr int MEM_MULTIPLIER = 2;
}

template <typename T, std::size_t chunkCapacity>
class UserAllocator{
public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = UserAllocator<U, chunkCapacity>;
    };

    UserAllocator() = default;
    ~UserAllocator() = default;

    template<typename U>
    UserAllocator(const UserAllocator<U, chunkCapacity>&){}

    pointer allocate(std::size_t n)
    {
        size_t quantity = 0;
        size_t requestedBytes = 0;
        if(!m_currentPointer && chunkCapacity)
            quantity  = chunkCapacity;
        else
            quantity  = n;

        requestedBytes = quantity * sizeof (value_type);

        if((requestedBytes <= (chunkCapacity*sizeof (value_type) - m_offset)) && m_currentPointer)
        {
            auto pValidMemory = m_currentPointer + m_offset/sizeof (value_type);
            m_offset += requestedBytes;
            return pValidMemory;
        }

        auto p = new value_type[requestedBytes/sizeof (value_type)];

        m_offset = n * sizeof(value_type);
        m_currentPointer = p;

        m_storage.push_back(m_currentPointer);

        return m_currentPointer;
    }

    void deallocate(pointer p, std::size_t n)
    {
        auto it = std::find(m_storage.begin(), m_storage.end(), p);
        if( it == m_storage.end())
            return;
        if(p == m_currentPointer)
            m_currentPointer = nullptr;
        m_storage.erase(it);
        delete [] p;
        m_offset -= sizeof (value_type) * n;
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new (p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p)
    {
        p->~value_type();
    }

private:
    std::vector<pointer> m_storage;
    std::size_t m_offset = 0;
    pointer m_currentPointer = nullptr;
};

