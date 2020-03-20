//
// Created by ali-masa on 3/20/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_UNROLLED_NODE_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_UNROLLED_NODE_H

#include <cstddef>

namespace soa {
    template<class T, unsigned int SIZE>
    class UltimateLinkedList;

    template<class T, unsigned int SIZE>
    class UnrolledNode {
        friend class soa::UltimateLinkedList<T, SIZE>;
    public:
        UnrolledNode<T, SIZE>(size_t capacity);
        UnrolledNode<T, SIZE>(UnrolledNode<T, SIZE> &toCopy);
        UnrolledNode<T, SIZE> &operator=(UnrolledNode<T, SIZE> &toCopy);
        virtual ~UnrolledNode<T, SIZE>();

        bool empty() const;
        size_t size() const;
        size_t capacity() const;
        T &operator[](size_t);
        T &at(size_t);

    private:
        T m_data[SIZE];
        size_t m_size;
        size_t m_capacity;
        UnrolledNode<T, SIZE>* m_next;
        UnrolledNode<T, SIZE>* m_prev;
    };
}

using namespace soa;

template<class T, unsigned int SIZE>
UnrolledNode<T, SIZE>::UnrolledNode(size_t capacity):m_capacity(m_capacity), m_size(0), m_next(NULL), m_prev(NULL){}

template<class T, unsigned int SIZE>
UnrolledNode<T, SIZE>::UnrolledNode(UnrolledNode<T, SIZE> &toCopy):m_capacity(m_capacity), m_size(0)
{
    while(this->m_size < toCopy.m_size)
    {
        this[this->m_size++] = toCopy[this->m_size];
    }

    m_next = toCopy.m_next;
    m_prev = toCopy.m_prev;
}

template<class T, unsigned int SIZE>
UnrolledNode<T, SIZE> &UnrolledNode<T, SIZE>::operator=(UnrolledNode<T, SIZE> &toCopy)
{
    if(this != &toCopy)
    {
        this->m_capacity = toCopy.m_capacity;
        this->m_size = 0;
        while(this->m_size < toCopy.m_size)
        {
            this[this->m_size++] = toCopy[this->m_size];
        }
    }

    m_next = toCopy.m_next;
    m_prev = toCopy.m_prev;

    return *this;
}

template<class T, unsigned int SIZE>
UnrolledNode<T, SIZE>::~UnrolledNode<T, SIZE>() {}

template<class T, unsigned int SIZE>
bool UnrolledNode<T, SIZE>::empty() const {
    return m_size;
}

template<class T, unsigned int SIZE>
size_t UnrolledNode<T, SIZE>::size() const {
    return m_size;
}

template<class T, unsigned int SIZE>
size_t UnrolledNode<T, SIZE>::capacity() const {
    return m_capacity;
}

template<class T, unsigned int SIZE>
T &UnrolledNode<T, SIZE>::operator[](size_t index) {
    if(index < m_capacity)
        return this->m_data[index];

    throw -1;// data not found
}

template<class T, unsigned int SIZE>
T &UnrolledNode<T, SIZE>::at(size_t index) {
    if(index < m_capacity)
        return this->m_data[index];

    throw -1;
}

#endif //THE_STATE_OF_THE_ART_LINKED_LIST_UNROLLED_NODE_H
