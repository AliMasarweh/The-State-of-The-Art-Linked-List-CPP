//
// Created by ali-masa on 3/18/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H

#include <cstddef>
#include "../PlatformIndependentConcurrency/mutex.h"
#include "../PlatformIndependentConcurrency/concurrency_abstract_factory.h"
#include "unrolled_node.h"

namespace soa {

    static ConcurrencyAbstractFactory* concurrencyAbstractFactory
                    = ConcurrencyAbstractFactory::getInstance();

    template<class T, unsigned int SIZE>
    class UltimateLinkedList;

    template <class T, unsigned int SIZE>
    class RealPopperPusher
    {
    public:
        virtual bool pushBack(UltimateLinkedList<T, SIZE>& ,T& value) = 0;
        virtual T popBack() = 0;
        virtual bool pushFront(T& value) = 0;
        virtual T popFront() = 0;

        virtual ~RealPopperPusher() {}
    };

    template<class T, unsigned int SIZE = 10>
    class UltimateLinkedList {

        friend class RealPopperPusher<T, SIZE>;

        friend bool operator==(const UltimateLinkedList<T, SIZE> &l1, const UltimateLinkedList<T, SIZE> &l2)
        {
            Iterator iterator1(l1);
            Iterator iterator2(l2);

            while(iterator1.hasNext() && iterator2.hasNext())
            {
                if(*iterator1 != *iterator2)
                    return false;
                ++iterator1;
                ++iterator2;
            }

            return !(iterator1.hasNext() || iterator2.hasNext());
        }

        friend bool operator!=(const UltimateLinkedList<T, SIZE> &l1, const UltimateLinkedList<T, SIZE> &l2)
        {
            return !(l1 == l2);
        }

    public:
        explicit UltimateLinkedList<T, SIZE>(size_t capacity);
        UltimateLinkedList<T, SIZE>(const UltimateLinkedList<T, SIZE> &toCopy);
        UltimateLinkedList<T, SIZE> &operator=(const UltimateLinkedList<T, SIZE> &toCopy);
        virtual ~UltimateLinkedList<T, SIZE>();

        class Iterator
        {
            friend class UltimateLinkedList<T, SIZE>;
        public:
            explicit Iterator(UltimateLinkedList<T, SIZE>* lstToIterate);
            Iterator operator++(int);
            Iterator& operator++();
            Iterator& next();
            bool hasNext();

        protected:
            UnrolledNode<T, SIZE>* operator->();
            UnrolledNode<T, SIZE>& operator*();

            const static Iterator s_end = NULL;
        private:
            UnrolledNode<T, SIZE>* m_pntrToTraverse;
            size_t m_indx;
        };
        /* this doesn't work: using iterator = UltimateLinkedList<T, SIZE>::Iterator;*/
        typedef UltimateLinkedList<T, SIZE>::Iterator iterator;
        iterator& begin();
        const iterator& cbegin() const;
        iterator& end();
        const iterator& cend() const;

        bool empty() const;
        size_t size() const;
        size_t capacity() const;

        bool push_back(const T &value);
        T pop_back();
        bool push_front(const T &value);
        T pop_front();

    private:
        UnrolledNode<T, SIZE> m_head;
        UnrolledNode<T, SIZE>* m_tail;
        // count
        size_t m_size;
        size_t m_capacity;

        Mutex* m_threadSafetyMutex;
        RealPopperPusher<T, SIZE>* m_realPopperPusher;
        iterator m_iterator;

        void releaseHeap();
    };
}
using namespace soa;


template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::UltimateLinkedList(size_t capacity) :m_capacity(capacity),
    m_head(capacity),m_tail(&m_head),m_iterator(this)
    ,m_threadSafetyMutex(concurrencyAbstractFactory->createMutex()){}

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::UltimateLinkedList(const UltimateLinkedList<T, SIZE> &toCopy) :m_capacity(toCopy.m_capacity)
        ,m_iterator(this)
        ,m_threadSafetyMutex(concurrencyAbstractFactory->createMutex())
{
    UnrolledNode<T, SIZE>* traverser = &m_head;
    UnrolledNode<T, SIZE>* traverserToCopy = & toCopy.m_head;

    *traverser = *traverserToCopy;

    while(traverserToCopy->m_next != NULL)
    {
        *traverser->m_next = *traverserToCopy->m_next;
        traverser = traverser->m_next;
        traverserToCopy = traverserToCopy->m_next;
    }
}

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE> &UltimateLinkedList<T, SIZE>::operator=(const UltimateLinkedList<T, SIZE> &toCopy)
{
    if(*this != toCopy)
    {
        this->releaseHeap();

        UnrolledNode<T, SIZE>* traverser = &m_head;
        UnrolledNode<T, SIZE>* traverserToCopy = & toCopy.m_head;

        *traverser = *traverserToCopy;

        while(traverserToCopy->m_next != NULL)
        {
            *traverser->m_next = *traverserToCopy->m_next;
            traverser = traverser->m_next;
            traverserToCopy = traverserToCopy->m_next;
        }
    }

    return *this;
}

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::~UltimateLinkedList<T, SIZE>()
{
    this->releaseHeap();
}

template<class T, unsigned int SIZE>
typename UltimateLinkedList<T, SIZE>::Iterator& UltimateLinkedList<T, SIZE>::begin()
{
    return m_iterator;
}

template<class T, unsigned int SIZE>
const typename UltimateLinkedList<T, SIZE>::Iterator& UltimateLinkedList<T, SIZE>::cbegin() const
{
    return m_iterator;
}

template<class T, unsigned int SIZE>
typename UltimateLinkedList<T, SIZE>::iterator& UltimateLinkedList<T, SIZE>::end()
{
    return UltimateLinkedList<T, SIZE>::iterator::s_end;
}

template<class T, unsigned int SIZE>
const typename UltimateLinkedList<T, SIZE>::iterator& UltimateLinkedList<T, SIZE>::cend() const
{
    return UltimateLinkedList<T, SIZE>::iterator::s_end;
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::empty() const
{
    return m_size;
}

template<class T, unsigned int SIZE>
size_t UltimateLinkedList<T, SIZE>::size() const
{
    return m_size;
}

template<class T, unsigned int SIZE>
size_t UltimateLinkedList<T, SIZE>::capacity() const
{
    return m_capacity;
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::push_back(const T &value)
{
    if(this->m_size == this->m_capacity)
        return false;

    m_threadSafetyMutex->lock();
    if(m_tail->m_size == m_tail->m_capacity)
    {
        m_tail->m_next = new UnrolledNode<T, SIZE>(m_tail->m_capacity);
        m_tail->m_next->m_prev = m_tail;
        m_tail = m_tail->m_next;
    }
    m_tail->at(m_tail->m_size ++) = value;
    m_threadSafetyMutex->unlock();

    return true;
}

template<class T, unsigned int SIZE>
T UltimateLinkedList<T, SIZE>::pop_back()
{
    Mutex::Lock lock(m_threadSafetyMutex);
    if(m_tail->m_size == 0)
    {
        m_tail = m_tail->m_prev;
        delete m_tail->m_next;
    }
    T ans = m_tail[m_tail->m_size--];

    return ans;
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::push_front(const T &value)
{
    if(this->m_size == 0)
        return false;
    Mutex::Lock lock(m_threadSafetyMutex);
    UnrolledNode<T, SIZE>* headCopy = new UnrolledNode<T, SIZE>(m_head);
    m_head.m_size = 0;
    m_head.m_next = headCopy;
    m_head.at(m_size++) = value;

    return true;
}

template<class T, unsigned int SIZE>
T UltimateLinkedList<T, SIZE>::pop_front()
{
    Mutex::Lock lock(m_threadSafetyMutex);
    UnrolledNode<T, SIZE> afterHeadCopy(m_head.m_next);
    if(m_head->m_size == 0)
    {
        delete m_head.m_next;
        m_head = m_head.m_next;
        m_head = afterHeadCopy;
    }
    T ans = m_head[m_head->m_size--];

    return ans;
}

template<class T, unsigned int SIZE>
void UltimateLinkedList<T, SIZE>::releaseHeap()
{
    while(m_tail != &m_head)
    {
        m_tail = m_tail->m_prev;
        delete m_tail->m_next;
    }
}

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::Iterator::Iterator(UltimateLinkedList<T, SIZE> *lstToIterate): m_indx(0),
        m_pntrToTraverse(& lstToIterate->m_head){}

template<class T, unsigned int SIZE>
typename UltimateLinkedList<T, SIZE>::Iterator UltimateLinkedList<T, SIZE>::Iterator::operator++(int)
{
    if(m_pntrToTraverse->m_size == m_pntrToTraverse->m_capacity)
        m_pntrToTraverse = m_pntrToTraverse->m_next;
    UltimateLinkedList<T, SIZE>::Iterator ans(*this);
    ++m_pntrToTraverse->m_size;

    return ans;
}

template<class T, unsigned int SIZE>
typename UltimateLinkedList<T, SIZE>::Iterator &UltimateLinkedList<T, SIZE>::Iterator::operator++()
{
    if(m_pntrToTraverse->m_size == m_pntrToTraverse->m_capacity)
        m_pntrToTraverse = m_pntrToTraverse->m_next;
    ++m_pntrToTraverse->m_size;

    return *this;
}

template<class T, unsigned int SIZE>
UnrolledNode<T, SIZE>* UltimateLinkedList<T, SIZE>::Iterator::operator->() {
    return m_pntrToTraverse;
}

template<class T, unsigned int SIZE>
UnrolledNode<T, SIZE> &UltimateLinkedList<T, SIZE>::Iterator::operator*() {
    return *m_pntrToTraverse;
}

template<class T, unsigned int SIZE>
typename UltimateLinkedList<T, SIZE>::Iterator &UltimateLinkedList<T, SIZE>::Iterator::next() {
    ++this;
    return *this;
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::Iterator::hasNext()
{
    return m_pntrToTraverse->m_size < m_pntrToTraverse->m_capacity ||
           m_pntrToTraverse->m_next != NULL;
}

#endif //THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
