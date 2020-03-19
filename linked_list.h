//
// Created by ali-masa on 3/18/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H

#include <cstddef>
#include "../PlatformIndependentConcurrency/mutex.h"
#include "../PlatformIndependentConcurrency/concurrency_abstract_factory.h"

namespace soa {

    static ConcurrencyAbstractFactory* concurrencyAbstractFactory
                    = ConcurrencyAbstractFactory::getInstance();

    template<class T, unsigned int SIZE>
    class LinkedList;

    template<class T, unsigned int SIZE>
    class Node {
        friend class soa::LinkedList<T, SIZE>;
    public:
        Node<T, SIZE>(size_t capacity);
        Node<T, SIZE>(Node<T, SIZE> &toCopy);
        Node<T, SIZE> &operator=(Node<T, SIZE> &toCopy);
        virtual ~Node<T, SIZE>();

        bool empty() const;
        size_t size() const;
        size_t capacity() const;
        T &operator[](size_t);
        T &at(size_t);

    private:
        T m_data[SIZE];
        size_t m_size;
        size_t m_capacity;
        Node<T, SIZE>* m_next;
        Node<T, SIZE>* m_prev;
    };

    template <class T>
    class PopperPusherProxy
    {
    public:
        virtual bool pushBack(T& value) = 0;
        virtual T popBack() = 0;
        virtual bool pushFront(T& value) = 0;
        virtual T popFront() = 0;

        virtual ~PopperPusherProxy() {}
    };

    template<class T, unsigned int SIZE = 10>
    class LinkedList {

        friend bool operator==(const LinkedList<T, SIZE> &l1, const LinkedList<T, SIZE> &l2)
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

        friend bool operator!=(const LinkedList<T, SIZE> &l1, const LinkedList<T, SIZE> &l2)
        {
            return !(l1 == l2);
        }

    public:
        explicit LinkedList<T, SIZE>(size_t capacity);
        LinkedList<T, SIZE>(const LinkedList<T, SIZE> &toCopy);
        LinkedList<T, SIZE> &operator=(const LinkedList<T, SIZE> &toCopy);
        virtual ~LinkedList<T, SIZE>();

        class Iterator
        {
            friend class LinkedList<T, SIZE>;
        public:
            explicit Iterator(LinkedList<T, SIZE>* lstToIterate);
            Iterator operator++(int);
            Iterator& operator++();
            Iterator& next();
            bool hasNext();

        protected:
            Node<T, SIZE>* operator->();
            Node<T, SIZE>& operator*();

            const static Iterator s_end = NULL;
        private:
            Node<T, SIZE>* m_pntrToTraverse;
            size_t m_indx;
        };
        using iterator = LinkedList<T, SIZE>::Iterator;
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
        Node<T, SIZE> m_head;
        Node<T, SIZE>* m_tail;
        // count
        size_t m_size;
        size_t m_capacity;

        Mutex* m_threadSafetyMutex;
        iterator m_iterator;

        void releaseHeap();
    };
}
using namespace soa;


template<class T, unsigned int SIZE>
LinkedList<T, SIZE>::LinkedList(size_t capacity) :m_capacity(capacity),
    m_head(capacity),m_tail(&m_head),m_iterator(this)
    ,m_threadSafetyMutex(concurrencyAbstractFactory->createMutex()){}

template<class T, unsigned int SIZE>
LinkedList<T, SIZE>::LinkedList(const LinkedList<T, SIZE> &toCopy) :m_capacity(toCopy.m_capacity)
        ,m_iterator(this)
        ,m_threadSafetyMutex(concurrencyAbstractFactory->createMutex())
{
    Node<T, SIZE>* traverser = &m_head;
    Node<T, SIZE>* traverserToCopy = & toCopy.m_head;

    *traverser = *traverserToCopy;

    while(traverserToCopy->m_next != NULL)
    {
        *traverser->m_next = *traverserToCopy->m_next;
        traverser = traverser->m_next;
        traverserToCopy = traverserToCopy->m_next;
    }
}

template<class T, unsigned int SIZE>
LinkedList<T, SIZE> &LinkedList<T, SIZE>::operator=(const LinkedList<T, SIZE> &toCopy)
{
    if(*this != toCopy)
    {
        this->releaseHeap();

        Node<T, SIZE>* traverser = &m_head;
        Node<T, SIZE>* traverserToCopy = & toCopy.m_head;

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
LinkedList<T, SIZE>::~LinkedList<T, SIZE>()
{
    this->releaseHeap();
}

template<class T, unsigned int SIZE>
typename LinkedList<T, SIZE>::Iterator& LinkedList<T, SIZE>::begin()
{
    return m_iterator;
}

template<class T, unsigned int SIZE>
const typename LinkedList<T, SIZE>::Iterator& LinkedList<T, SIZE>::cbegin() const
{
    return m_iterator;
}

template<class T, unsigned int SIZE>
typename LinkedList<T, SIZE>::iterator& LinkedList<T, SIZE>::end()
{
    return LinkedList<T, SIZE>::iterator::s_end;
}

template<class T, unsigned int SIZE>
const typename LinkedList<T, SIZE>::iterator& LinkedList<T, SIZE>::cend() const
{
    return LinkedList<T, SIZE>::iterator::s_end;
}

template<class T, unsigned int SIZE>
bool LinkedList<T, SIZE>::empty() const
{
    return m_size;
}

template<class T, unsigned int SIZE>
size_t LinkedList<T, SIZE>::size() const
{
    return m_size;
}

template<class T, unsigned int SIZE>
size_t LinkedList<T, SIZE>::capacity() const
{
    return m_capacity;
}

template<class T, unsigned int SIZE>
bool LinkedList<T, SIZE>::push_back(const T &value)
{
    if(this->m_size == this->m_capacity)
        return false;

    m_threadSafetyMutex->lock();
    if(m_tail->m_size == m_tail->m_capacity)
    {
        m_tail->m_next = new Node<T, SIZE>(m_tail->m_capacity);
        m_tail->m_next->m_prev = m_tail;
        m_tail = m_tail->m_next;
    }
    m_tail->at(m_tail->m_size ++) = value;
    m_threadSafetyMutex->unlock();

    return true;
}

template<class T, unsigned int SIZE>
T LinkedList<T, SIZE>::pop_back()
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
bool LinkedList<T, SIZE>::push_front(const T &value)
{
    if(this->m_size == 0)
        return false;
    Mutex::Lock lock(m_threadSafetyMutex);
    Node<T, SIZE>* headCopy = new Node<T, SIZE>(m_head);
    m_head.m_size = 0;
    m_head.m_next = headCopy;
    m_head.at(m_size++) = value;

    return true;
}

template<class T, unsigned int SIZE>
T LinkedList<T, SIZE>::pop_front()
{
    Mutex::Lock lock(m_threadSafetyMutex);
    Node<T, SIZE> afterHeadCopy(m_head.m_next);
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
void LinkedList<T, SIZE>::releaseHeap()
{
    while(m_tail != &m_head)
    {
        m_tail = m_tail->m_prev;
        delete m_tail->m_next;
    }
}

template<class T, unsigned int SIZE>
LinkedList<T, SIZE>::Iterator::Iterator(LinkedList<T, SIZE> *lstToIterate): m_indx(0),
        m_pntrToTraverse(& lstToIterate->m_head){}

template<class T, unsigned int SIZE>
typename LinkedList<T, SIZE>::Iterator LinkedList<T, SIZE>::Iterator::operator++(int)
{
    if(m_pntrToTraverse->m_size == m_pntrToTraverse->m_capacity)
        m_pntrToTraverse = m_pntrToTraverse->m_next;
    LinkedList<T, SIZE>::Iterator ans(*this);
    ++m_pntrToTraverse->m_size;

    return ans;
}

template<class T, unsigned int SIZE>
typename LinkedList<T, SIZE>::Iterator &LinkedList<T, SIZE>::Iterator::operator++()
{
    if(m_pntrToTraverse->m_size == m_pntrToTraverse->m_capacity)
        m_pntrToTraverse = m_pntrToTraverse->m_next;
    ++m_pntrToTraverse->m_size;

    return *this;
}

template<class T, unsigned int SIZE>
Node<T, SIZE>* LinkedList<T, SIZE>::Iterator::operator->() {
    return m_pntrToTraverse;
}

template<class T, unsigned int SIZE>
Node<T, SIZE> &LinkedList<T, SIZE>::Iterator::operator*() {
    return *m_pntrToTraverse;
}

template<class T, unsigned int SIZE>
typename LinkedList<T, SIZE>::Iterator &LinkedList<T, SIZE>::Iterator::next() {
    ++this;
    return *this;
}

template<class T, unsigned int SIZE>
bool LinkedList<T, SIZE>::Iterator::hasNext()
{
    return m_pntrToTraverse->m_size < m_pntrToTraverse->m_capacity ||
           m_pntrToTraverse->m_next != NULL;
}

template<class T, unsigned int SIZE>
Node<T, SIZE>::Node(size_t capacity):m_capacity(m_capacity), m_size(0), m_next(NULL), m_prev(NULL){}

template<class T, unsigned int SIZE>
Node<T, SIZE>::Node(Node<T, SIZE> &toCopy):m_capacity(m_capacity), m_size(0)
{
    while(this->m_size < toCopy.m_size)
    {
        this[this->m_size++] = toCopy[this->m_size];
    }

    m_next = toCopy.m_next;
    m_prev = toCopy.m_prev;
}

template<class T, unsigned int SIZE>
Node<T, SIZE> &Node<T, SIZE>::operator=(Node<T, SIZE> &toCopy)
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
Node<T, SIZE>::~Node<T, SIZE>() {}

template<class T, unsigned int SIZE>
bool Node<T, SIZE>::empty() const {
    return m_size;
}

template<class T, unsigned int SIZE>
size_t Node<T, SIZE>::size() const {
    return m_size;
}

template<class T, unsigned int SIZE>
size_t Node<T, SIZE>::capacity() const {
    return m_capacity;
}

template<class T, unsigned int SIZE>
T &Node<T, SIZE>::operator[](size_t index) {
    if(index < m_capacity)
        return this->m_data[index];

    throw -1;// data not found
}

template<class T, unsigned int SIZE>
T &Node<T, SIZE>::at(size_t index) {
    if(index < m_capacity)
        return this->m_data[index];

    throw -1;
}

#endif //THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
