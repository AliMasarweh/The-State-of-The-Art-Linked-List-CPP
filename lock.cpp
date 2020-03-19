//
// Created by ali-masa on 3/19/20.
//

#include "lock.h"

Lock::Lock(Mutex *mutex)
{
    m_mutex = mutex;
    m_mutex->lock();
}

Lock::~Lock()
{
    m_mutex->unlock();
}
