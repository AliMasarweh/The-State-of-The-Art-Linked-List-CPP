//
// Created by ali-masa on 3/18/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H

#include <cstddef>
#include "../PlatformIndependentConcurrency/mutex.h"

namespace soa {

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

        const T &operator[](size_t) const;

        T &at(size_t);

    private:
        T m_data[SIZE];
        size_t m_size;
        size_t m_capacity;
        Node<T, SIZE>* m_next;
    };

    template<class T, unsigned int SIZE>
    class LinkedList {

        friend bool operator<(const LinkedList<T, SIZE> &l1, const LinkedList<T, SIZE> &l2);
        friend bool operator==(const LinkedList<T, SIZE> &l1, const LinkedList<T, SIZE> &l2);
        friend bool operator!=(const LinkedList<T, SIZE> &l1, const LinkedList<T, SIZE> &l2);

    public:

        explicit LinkedList<T, SIZE>(size_t capacity);

        LinkedList<T, SIZE>(const LinkedList<T, SIZE> &toCopy);

        LinkedList<T, SIZE> &operator=(const LinkedList<T, SIZE> &toCopy);

        virtual ~LinkedList<T, SIZE>();

        T &front();

        T &back();

        class Iterator
        {
            friend class LinkedList<T, SIZE>;
        public:
            explicit Iterator(LinkedList<T, SIZE>* lstToIterate);
            Iterator& operator++(int);
            Iterator& operator++();
            Node<T, SIZE>& operator->();
            Node<T, SIZE>& operator*();
            bool haseNext();

            const static Iterator s_end = NULL;
        private:
            Node<T, SIZE>* m_pntrToTraverse;
        };

        using iterator = LinkedList<T, SIZE>::Iterator;

        iterator& begin();

        const iterator& cbegin() const;

        iterator& end();

        const iterator& cend() const;

        bool empty() const;

        size_t size() const;

        size_t capacity() const;

        iterator& insert(iterator& pos, const T &value);

        iterator& erase(iterator& pos);

        void push_back(const T &value);

        void pop_back();

        void push_front(const T &value);

        void pop_front();

        void resize(size_t count, T value = T());

    private:
        Node<T, SIZE> m_head;
        Node<T, SIZE>* m_tail;
        // count
        size_t m_size;
        size_t m_capacity;
        Mutex *m_mutex;

        iterator m_iterator;

        void shiftToRight(iterator &pos);
    };
}
    using namespace soa;


    template<class T, unsigned int SIZE>
    LinkedList<T, SIZE>::LinkedList(size_t capacity) :m_capacity(capacity),
    m_head(capacity),m_tail(&m_head),m_iterator(this) {}

    template<class T, unsigned int SIZE>
    LinkedList<T, SIZE>::LinkedList(const LinkedList<T, SIZE> &toCopy) :m_capacity(toCopy.m_capacity)
            ,m_iterator(this)
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
//            release();

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
//        release();
    }

    template<class T, unsigned int SIZE>
    T &LinkedList<T, SIZE>::front()
    {
        return m_head[0];
    }

    template<class T, unsigned int SIZE>
    T &LinkedList<T, SIZE>::back()
    {
        return m_tail[m_tail->m_size -1 ];
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

    template <class T, unsigned int SIZE>
    void LinkedList<T, SIZE>::shiftToRight(LinkedList::iterator& pos)
    {
        T next = pos->m_data;
        T current;
        while(pos.haseNext())
        {
            current = next;
            pos = pos->m_next;
            next = pos->m_data;
            pos->m_data = current;
        }
    }

    template<class T, unsigned int SIZE>
    typename LinkedList<T, SIZE>::iterator&
        LinkedList<T, SIZE>::insert(LinkedList::iterator& pos, const T &value)
    {
        iterator tmp = iterator(pos);
        this->shiftToRight(tmp);
        pos->m_data = value;
        return ++pos;
    }

    template<class T, unsigned int SIZE>
    LinkedList::iterator LinkedList<T, SIZE>::erase(LinkedList::iterator& pos)
    {
        for (int i = pos.operator->().size() - 1; i < ; ++i)
        {

        }
        pos.operator->().at()
        return soa::LinkedList::iterator();
    }

    template<class T, unsigned int SIZE>
    void LinkedList<T, SIZE>::push_back(const T &value)
    {

    }

    template<class T, unsigned int SIZE>
    void LinkedList<T, SIZE>::pop_back()
    {

    }

    template<class T, unsigned int SIZE>
    void LinkedList<T, SIZE>::push_front(const T &value)
    {

    }

    template<class T, unsigned int SIZE>
    void LinkedList<T, SIZE>::pop_front()
    {

    }

    template<class T, unsigned int SIZE>
    void LinkedList<T, SIZE>::resize(size_t count, T value)
    {

    }


#endif //THE_STATE_OF_THE_ART_LINKED_LIST_LINKED_LIST_H
