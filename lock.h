//
// Created by ali-masa on 3/19/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_LOCK_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_LOCK_H


#include "../PlatformIndependentConcurrency/mutex.h"

class Lock
{
public:
    Lock(Mutex* mutex);
    ~Lock();
private:
    Mutex* m_mutex;
};


#endif //THE_STATE_OF_THE_ART_LINKED_LIST_LOCK_H
