//
// Created by ali-masa on 3/20/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_REAL_POPPER_PUSHER_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_REAL_POPPER_PUSHER_H

namespace soa {
    template<class T, unsigned int SIZE>
    class UltimateLinkedList;

    template<class T, unsigned int SIZE>
    class RealPopperPusher {
    public:
        virtual bool pushBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList, T &value) = 0;

        virtual T popBack(UltimateLinkedList<T, SIZE> &ultimateLinkedList) = 0;

        virtual bool pushFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList, T &value) = 0;

        virtual T popFront(UltimateLinkedList<T, SIZE> &ultimateLinkedList) = 0;

        virtual ~RealPopperPusher() {}
    };
}



#endif //THE_STATE_OF_THE_ART_LINKED_LIST_REAL_POPPER_PUSHER_H
