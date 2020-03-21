#include <iostream>
#include <gtest/gtest.h>
#include <time.h>

#include "ultimate_linked_list.h"

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

using namespace std;

string stringedArrayInRange(int begin, int end)
{
    stringstream ss;
    ss << "[" << begin;
    for (int i = begin + 1; i < end; ++i)
    {
        ss << ", " << i;
    }
    ss << "]";

    return ss.str();
}

TEST(UnrolledNodeBasicTests, UnrolledNodeBasicOperations)
{
    const unsigned short nodeCapacity = 7;
    const unsigned short templateSize = 10;
    UnrolledNode<int, templateSize> node(nodeCapacity);
    ASSERT_EQ(node.capacity(), nodeCapacity);
    ASSERT_EQ(node.empty(), true);

    node.at(0) = 0;
    ASSERT_EQ(node.empty(), false);
    ASSERT_EQ(node.size(), 1);
}

TEST(UnrolledNodeAdvancedTests, UnrolledNodeAdvancedOperations) {
    const unsigned short nodeCapacity = 7;
    const unsigned short templateSize = 10;
    UnrolledNode<int, templateSize> node(nodeCapacity);

    for (int i = 0; i < nodeCapacity; ++i) {
        node.at(i) = i;
        ASSERT_EQ(node.size(), i + 1);
    }

    stringstream ss;
    ss << node;
    ASSERT_EQ(ss.str(), stringedArrayInRange(0, nodeCapacity));

    EXPECT_THROW(node[nodeCapacity], InvalidIndexUnrolledNode);

    UnrolledNode<int, templateSize> copy(node);
    stringstream ss2;
    ss2 << copy;
    ASSERT_EQ(ss.str(), ss2.str());
}

TEST(UnrolledNodeAdvancedTests, UnrolledNodeAdvancedOperationsWithRandomNumbers)
{
    const unsigned short nodeCapacity = 7;
    const unsigned short templateSize = 10;
    const unsigned short randomMaxNumber = 1001;
    const unsigned short loopsNumber = 100;

    UnrolledNode<int, templateSize> node(nodeCapacity);
    UnrolledNode<int, templateSize> copyByAssignmentOperator(nodeCapacity/2);
    copyByAssignmentOperator = node;
    ASSERT_EQ(copyByAssignmentOperator.capacity(), node.capacity());
    stringstream ss, ss2;

    for (int j = 0; j < loopsNumber; ++j)
    {
        for (int i = 0; i < nodeCapacity; ++i)
        {
            node.at(i) = rand()%randomMaxNumber;
        }
        copyByAssignmentOperator = node;
        ss << node;
        ss2 << copyByAssignmentOperator;
        ASSERT_EQ(ss.str(), ss2.str());
        ss.clear();
        ss2.clear();
    }
}

string stringedSplittedArrayInRange(int begin, int end, int splits)
{
    stringstream ss;
    int jump = (end - begin)/splits;
    ss << "[" << stringedArrayInRange(begin, begin + jump);
    for (int i = begin + jump; i < end; i += jump)
    {
        ss << ", " << stringedArrayInRange(i, min(end, i+jump));
    }
    ss << "]";

    return ss.str();
}

TEST(UltimateLinkedListBasicTests, UltimateLinkedListBasicOperations)
{
    const unsigned short templateSize = 10;
    const unsigned short listCapacity = 30;
    UltimateLinkedList<int, templateSize> ultimateLinkedList(listCapacity);

    ASSERT_EQ(ultimateLinkedList.capacity(), listCapacity);
    ASSERT_EQ(ultimateLinkedList.empty(), true);

    ultimateLinkedList.push_back(0);
    ASSERT_EQ(ultimateLinkedList.empty(), false);
    ASSERT_EQ(ultimateLinkedList.size(), 1);
}

TEST(UltimateLinkedListBasicTests, UltimateLinkedListBasicPushing) {
    const unsigned short listCapacity = 7;
    const unsigned short templateSize = 10;
    UltimateLinkedList<int, templateSize> ultimateLinkedList(listCapacity);

    for (int i = 0; i < listCapacity; ++i)
    {
        ultimateLinkedList.push_back(i);
        ASSERT_EQ(ultimateLinkedList.size(), i + 1);
    }

    stringstream ss;
    ss << ultimateLinkedList;
    ASSERT_EQ(ss.str(), stringedSplittedArrayInRange(0, listCapacity, 1));

    ASSERT_EQ(ultimateLinkedList.push_front(1), false);

    UltimateLinkedList<int, templateSize> copy(ultimateLinkedList);
    stringstream ss2;
    ss2 << copy;
    ASSERT_EQ(ss.str(), ss2.str());
}

TEST(UltimateLinkedListBasicTests, UltimateLinkedListPushingBackAndFront) {
    const unsigned short listCapacity = 10;
    const unsigned short templateSize = 10;
    UltimateLinkedList<int, templateSize> ultimateLinkedList(listCapacity);

    for (int i = 0; i < (listCapacity+1)/2; ++i)
    {
        ultimateLinkedList.push_back(i+(listCapacity+1)/2);
        ASSERT_EQ(ultimateLinkedList.size(), i + 1);
    }

    stringstream ss;
    ss << ultimateLinkedList;
    ASSERT_EQ(ss.str(), stringedSplittedArrayInRange(5, listCapacity, 1));
    ss.clear();


    int num = (listCapacity+1)/2;
    for (int i = (listCapacity+1)/2; i < listCapacity; ++i)
    {
        ultimateLinkedList.push_front(--num);
        ASSERT_EQ(ultimateLinkedList.size(), i + 1);
    }

    stringstream ss2;
    ss2 << ultimateLinkedList;
    ASSERT_EQ(ss2.str(), stringedSplittedArrayInRange(0, listCapacity, 1));

    ASSERT_EQ(ultimateLinkedList.push_front(1), false);

    UltimateLinkedList<int, templateSize> copy(ultimateLinkedList);
    stringstream ss3;
    ss3 << copy;
    ASSERT_EQ(ss2.str(), ss3.str());
}

TEST(UltimateLinkedListAdvancedTests, UltimateLinkedListAdvancedOperations) {
    const unsigned short listCapacity = 7;
    const unsigned short templateSize = 10;
    UltimateLinkedList<int, templateSize> ultimateLinkedList(listCapacity);

    for (int i = 0; i < listCapacity; ++i)
    {
        ultimateLinkedList.push_back(i);
        ASSERT_EQ(ultimateLinkedList.size(), i + 1);
    }

    stringstream ss;
    ss << ultimateLinkedList;
    ASSERT_EQ(ss.str(), stringedSplittedArrayInRange(0, listCapacity, 1));

    ASSERT_EQ(ultimateLinkedList.push_front(1), false);

    UltimateLinkedList<int, templateSize> copy(ultimateLinkedList);
    stringstream ss2;
    ss2 << copy;
    ASSERT_EQ(ss.str(), ss2.str());
}
