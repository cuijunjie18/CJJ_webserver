#include "timer/lst_timer.hpp"
#include <gtest/gtest.h>

class SortTimerListTest : public ::testing::Test {
public:
    void SetUp(){
        // Initialize the timer list with some timers
        for (int i = 1; i <= 5; i++) {
            UtilTimer* timer = new UtilTimer;
            timer->expire = i * 10; // Set expire times: 10, 20, 30, 40, 50
            timer_list.add_timer(timer);
        }
    }
    void TearDown(){
        // call destructor automatically
    }
public:
    SortTimerList timer_list;   // Tested list
};

class UtilTimerTest : public ::testing::Test {
    // Additional tests for UtilTimer can be added here
};

TEST_F(SortTimerListTest, add_timer) {
    // add middle timer
    UtilTimer* timer = new UtilTimer;
    timer->expire = 25;
    timer_list.add_timer(timer);

    UtilTimer* p = timer_list.get_head();
    std::vector<int> expires;
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    std::vector<int> expected = {10, 20, 25, 30, 40, 50};
    EXPECT_EQ(expected, expires);

    // add head timer
    timer = new UtilTimer;
    timer->expire = 5;
    timer_list.add_timer(timer);
    p = timer_list.get_head();
    EXPECT_EQ(p,timer);
    expires.clear();
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    expected = {5, 10, 20, 25, 30, 40, 50};
    EXPECT_EQ(expected, expires);

    // add tail timer
    timer = new UtilTimer;
    timer->expire = 55;
    timer_list.add_timer(timer);
    p = timer_list.get_head();
    expires.clear();
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    expected = {5, 10, 20, 25, 30, 40, 50, 55};
    EXPECT_EQ(timer, timer_list.get_tail());
    EXPECT_EQ(expected, expires);
}

TEST_F(SortTimerListTest, del_timer) {
    // delete head timer
    UtilTimer* head_timer = timer_list.get_head();
    timer_list.del_timer(head_timer);
    UtilTimer* p = timer_list.get_head();
    EXPECT_NE(p, head_timer);
    std::vector<int> expires;
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    std::vector<int> expected = {20, 30, 40, 50};
    EXPECT_EQ(expected, expires);

    // delete tail timer
    UtilTimer* tail_timer = timer_list.get_tail();
    timer_list.del_timer(tail_timer);
    p = timer_list.get_head();
    expires.clear();
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    expected = {20, 30, 40};
    EXPECT_EQ(expected, expires);

    // delete middle timer
    UtilTimer* middle_timer = timer_list.get_head()->next; // timer with expire 30
    timer_list.del_timer(middle_timer);
    p = timer_list.get_head();
    expires.clear();
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    expected = {20, 40};
    EXPECT_EQ(expected, expires);
}

TEST_F(SortTimerListTest, adjust_timer) {
    // adjust middle timer to later time
    UtilTimer* middle_timer = timer_list.get_head()->next; // timer with expire 20
    middle_timer->expire = 45;
    timer_list.adjust_timer(middle_timer);
    UtilTimer* p = timer_list.get_head();
    std::vector<int> expires;
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    std::vector<int> expected = {10, 30, 40, 45, 50};
    EXPECT_EQ(expected, expires);

    // adjust head timer to later time
    UtilTimer* head_timer = timer_list.get_head(); // timer with expire 10
    UtilTimer* head_next = head_timer->next;
    head_timer->expire = 35;
    timer_list.adjust_timer(head_timer);
    p = timer_list.get_head();
    EXPECT_EQ(p, head_next); // new head should be the old second timer
    expires.clear();
    while (p) {
        expires.push_back(p->expire);
        p = p->next;
    }
    expected = {30, 35, 40, 45, 50};
    EXPECT_EQ(expected, expires);
}

int main(int argc,char *argv[])
{
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}