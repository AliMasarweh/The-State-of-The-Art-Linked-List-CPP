//
// Created by ali-masa on 3/18/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_ULTIMATE_LINKED_LIST_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_ULTIMATE_LINKED_LIST_H

#include <cstddef>
#include "../PlatformIndependentConcurrency/mutex.h"
#include "../PlatformIndependentConcurrency/concurrency_abstract_factory.h"
#include "unrolled_node.h"

namespace soa {

    static ConcurrencyAbstractFactory* concurrencyFactory
                    = ConcurrencyAbstractFactory::getInstance();

    template<class T, unsigned int SIZE> class UltimateLinkedList;

    template<class T, unsigned int SIZE>
    class PopperPusher {
    public:
        virtual bool pushBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList, const T &value) = 0;
        virtual T popBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList) = 0;
        virtual bool pushFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList, const T &value) = 0;
        virtual T popFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList) = 0;
        virtual ~PopperPusher() {}
    };
    template<class T, unsigned int SIZE> class NormalPopperPusher;
    template<class T, unsigned int SIZE> class BlockingPopperPusher;

    template<class T, unsigned int SIZE = 10>
    class UltimateLinkedList {

        friend class PopperPusher<T, SIZE>;
        friend class NormalPopperPusher<T, SIZE>;
        friend class BlockingPopperPusher<T, SIZE>;

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

        friend std::ostream& operator<<(std::ostream& os, UltimateLinkedList<T, SIZE>& list)
        {
            UnrolledNode<T, SIZE>* travser = list.m_head;
            os << "[" << *travser;
            std::stringstream ss;
            ss << *travser;
            std::string str = ss.str();
            travser = &travser->getNext();
            while (travser != NULL)
            {
                os << ", " << *travser;
                str = ss.str();
                travser = &travser->getNext();
            }
            os << "]";

            return os;
        }

    public:
        explicit UltimateLinkedList<T, SIZE>(size_t capacity);
        UltimateLinkedList<T, SIZE>(const UltimateLinkedList<T, SIZE> &toCopy);
        UltimateLinkedList<T, SIZE> &operator=(const UltimateLinkedList<T, SIZE> &toCopy);
        virtual ~UltimateLinkedList<T, SIZE>();

        class Iterator
        {
            friend class UltimateLinkedList<T, SIZE>;
            friend bool operator==(Iterator& it1, Iterator& it2)
            {
                return (it1.m_pntrToTraverse == NULL && it2.m_pntrToTraverse == NULL)
                        || *(it1.m_pntrToTraverse) == *(it2.m_pntrToTraverse);
            }
            friend bool operator!=(Iterator& it1, Iterator& it2)
            { return !(it1 == it2); }
        public:
            explicit Iterator(UltimateLinkedList<T, SIZE>* lstToIterate);
            Iterator operator++(int);
            Iterator& operator++();
            Iterator& next();
            bool hasNext();

        protected:
            UnrolledNode<T, SIZE>* operator->();
            UnrolledNode<T, SIZE>& operator*();

            const static Iterator s_end;
        private:
            Iterator(void* pntr = NULL) :m_indx(-1), m_pntrToTraverse(NULL) {}
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
        void setWaitable(bool isWaitable);

    private:
        UnrolledNode<T, SIZE>* m_head;
        UnrolledNode<T, SIZE>* m_tail;
        // count
        size_t m_size;
        size_t m_nodeSize;
        size_t m_capacity;

        Mutex* m_threadSafetyMutex;
        PopperPusher<T, SIZE>* m_popperPusher;
        iterator m_iterator;

        bool innerPushBack(const T &value);
        T innerPopBack();
        bool innerPushFront(const T &value);
        T innerPopFront();
        void releaseHeapAllocations();
    };

    template<class T, unsigned int SIZE>
    class NormalPopperPusher: public PopperPusher<T, SIZE> {
    public:
        virtual bool pushBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList, const T &value)
        {   return ultimateLinkedList.innerPushBack(value);    }
        virtual T popBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList)
        {   return ultimateLinkedList.innerPopBack();    }
        virtual bool pushFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList, const T &value)
        {   return ultimateLinkedList.innerPushFront(value);    }
        virtual T popFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList)
        {   return ultimateLinkedList.innerPopFront();    }
        virtual ~NormalPopperPusher() {}
    };

    template<class T, unsigned int SIZE>
    class BlockingPopperPusher: public PopperPusher<T, SIZE> {
    public:
        BlockingPopperPusher(UltimateLinkedList<T, SIZE> &ultimateLinkedList)
        :m_addingSem(concurrencyFactory->createSemaphore(ultimateLinkedList.capacity()))
        , m_removingSem(concurrencyFactory->createSemaphore(0)){}
        virtual bool pushBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList, const T &value)
        {
            m_addingSem->wait();
            bool isSuccessful = ultimateLinkedList.innerPushBack(value);
            m_removingSem->post();

            return isSuccessful;
        }
        virtual T popBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList)
        {
            m_removingSem->wait();
            T backValue = ultimateLinkedList.innerPopBack();
            m_addingSem->post();

            return backValue;
        }
        virtual bool pushFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList, const T &value)
        {
            m_addingSem->wait();
            bool isSuccessful = ultimateLinkedList.innerPushFront(value);
            m_removingSem->post();

            return isSuccessful;
        }
        virtual T popFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList)
        {
            m_removingSem->wait();
            T frontValue = ultimateLinkedList.innerPopFront();
            m_addingSem->post();

            return frontValue;
        }
        virtual ~BlockingPopperPusher()
        {
            delete m_addingSem;
            delete m_removingSem;
        }

    private:
        Semaphore* m_addingSem;
        Semaphore* m_removingSem;
    };
}
using namespace soa;

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::UltimateLinkedList(size_t capacity) :m_capacity(capacity),
     m_iterator(this),m_size(0),m_nodeSize(SIZE)
    ,m_threadSafetyMutex(concurrencyFactory->createMutex())
    ,m_popperPusher(new NormalPopperPusher<T, SIZE>)
    {
        m_head = new UnrolledNode<T, SIZE>(m_nodeSize);
        m_tail = m_head;
    }

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::UltimateLinkedList(const UltimateLinkedList<T, SIZE> &toCopy)
        :m_capacity(toCopy.m_capacity),m_nodeSize(SIZE),m_iterator(this),
         m_size(toCopy.m_size),m_threadSafetyMutex(concurrencyFactory->createMutex())
        ,m_popperPusher(new NormalPopperPusher<T, SIZE>)
{
    m_head = new UnrolledNode<T, SIZE>(m_nodeSize);
    m_tail = m_head;
    UnrolledNode<T, SIZE>* traverser = m_head;
    const UnrolledNode<T, SIZE>* traverserToCopy = (toCopy.m_head);

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
        this->releaseHeapAllocations();

        *m_popperPusher = *toCopy.m_popperPusher;
        UnrolledNode<T, SIZE>* traverser = m_head;
        UnrolledNode<T, SIZE>* traverserToCopy = toCopy.m_head;

        *traverser = *traverserToCopy;

        while(traverserToCopy->m_next != NULL)
        {
            *traverser->m_next = *traverserToCopy->m_next;
            traverser = traverser->m_next;
            traverserToCopy = traverserToCopy->m_next;
        }

        m_tail = traverser;
    }

    return *this;
}

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::~UltimateLinkedList<T, SIZE>()
{
    this->releaseHeapAllocations();
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
    return m_size == 0;
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
    Mutex::Lock lock(m_threadSafetyMutex);
    return this->m_popperPusher->pushBack(*this, value);
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::innerPushBack(const T &value) {
    if(this->m_size == this->m_capacity)
        return false;

    if(m_tail->m_size == m_tail->m_capacity)
    {
        m_tail->m_next = new UnrolledNode<T, SIZE>(m_tail->m_capacity);
        m_tail->m_next->m_prev = m_tail;
        m_tail = m_tail->m_next;
    }
    m_tail->at(m_tail->m_size ++) = value;

    ++ this->m_size;
    return true;
}

template<class T, unsigned int SIZE>
T UltimateLinkedList<T, SIZE>::pop_back()
{
    Mutex::Lock lock(m_threadSafetyMutex);
    return this->m_popperPusher->popBack(*this);
}

template<class T, unsigned int SIZE>
T UltimateLinkedList<T, SIZE>::innerPopBack()
{
    if(m_size == 0)
        return T();
    if(m_tail->m_size == 0)
    {
        m_tail = m_tail->m_prev;
        delete m_tail->m_next;
        m_tail->m_next = NULL;
    }
    T ans = m_tail->at(m_tail->m_size--);
    -- this->m_size;

    return ans;
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::push_front(const T &value)
{
    Mutex::Lock lock(m_threadSafetyMutex);
    return this->m_popperPusher->pushFront(*this, value);
}

template<class T, unsigned int SIZE>
bool UltimateLinkedList<T, SIZE>::innerPushFront(const T &value) {
    if(this->m_size == this->m_capacity)
        return false;

    if(!m_head->push_front(value))
    {
        m_head->m_prev = new UnrolledNode<T, SIZE>(m_head->m_capacity);
        m_head->m_prev->m_next = m_head;
        m_head = m_head->m_prev;
    }

    ++ this->m_size;
    return true;
}


template<class T, unsigned int SIZE>
T UltimateLinkedList<T, SIZE>::pop_front()
{
    Mutex::Lock lock(m_threadSafetyMutex);
    return this->m_popperPusher->popFront(*this);
}

template<class T, unsigned int SIZE>
T UltimateLinkedList<T, SIZE>::innerPopFront() {
    UnrolledNode<T, SIZE> afterHeadCopy(*(m_head->m_next));
    if(m_size == 0)
        return T();

    if(m_head->m_size == 0)
    {
        m_head = m_head->m_next;
        delete m_head->m_prev;
        m_head->m_prev = NULL;
    }
    T ans = m_head->pop_front();
    -- this->m_size;

    return ans;
}


template<class T, unsigned int SIZE>
void UltimateLinkedList<T, SIZE>::setWaitable(bool isWaitable)
{
    delete this->m_popperPusher;
    if(isWaitable)
        m_popperPusher = new BlockingPopperPusher<T, SIZE>(*this);
    else
        m_popperPusher = new NormalPopperPusher<T, SIZE>;
}

template<class T, unsigned int SIZE>
void UltimateLinkedList<T, SIZE>::releaseHeapAllocations()
{
    while(m_tail != m_head)
    {
        m_tail = m_tail->m_prev;
        delete m_tail->m_next;
    }

    delete m_head;
    delete m_popperPusher;
}

template<class T, unsigned int SIZE>
UltimateLinkedList<T, SIZE>::Iterator::Iterator(UltimateLinkedList<T, SIZE> *lstToIterate): m_indx(0),
        m_pntrToTraverse(lstToIterate->m_head){}

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

#endif //THE_STATE_OF_THE_ART_LINKED_LIST_ULTIMATE_LINKED_LIST_H
