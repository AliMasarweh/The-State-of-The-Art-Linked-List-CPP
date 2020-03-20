//
// Created by ali-masa on 3/20/20.
//

#ifndef THE_STATE_OF_THE_ART_LINKED_LIST_ULTIMATE_LIST_EXCEPTION_H
#define THE_STATE_OF_THE_ART_LINKED_LIST_ULTIMATE_LIST_EXCEPTION_H


#include <exception>
#include <string>
namespace soa
{
    class UnrolledNodeException : public std::exception {
    public:
        UnrolledNodeException(std::string message) :m_message(message) {}
        virtual const char *what() const throw() { return m_message.c_str(); }
        ~UnrolledNodeException() throw() {}

    private:
        std::string m_message;
    };

    class InvalidIndexUnrolledNode: public UnrolledNodeException
    {
    public:
        InvalidIndexUnrolledNode(): UnrolledNodeException("Invalid Index in unrolled node!") {}
    };
}


#endif //THE_STATE_OF_THE_ART_LINKED_LIST_ULTIMATE_LIST_EXCEPTION_H
