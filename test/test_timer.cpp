#include <gtest/gtest.h>
#include <timer/lst_timer.hpp>
#include <bits/stdc++.h>

std::unordered_map<int,UtilTimer*> hash_map; 

SortTimerList timer_list;   // Tested list
std::list<int> ans_list;    // ans list

UtilTimer* x;

int input;
int n;

void init(){
    std::cin >> n;
    for (int i = 0; i < n; i++){
        x = new UtilTimer;
        x->expire = (i + 1) * 10;
        hash_map[x->expire] = x;
        timer_list.add_timer(x);
    }
}

void TimerList_check(){
    UtilTimer* p = timer_list.get_head();
    std::cin >> n;
    int next_n = 0;
    int prev_n = 0;
    while (p){
        next_n++;
        p = p->next;
    }
    p = timer_list.get_tail();
    while (p){
        prev_n++;
        p = p->prev;
    }
    EXPECT_EQ(n, next_n) << "Num of list unmatch!";
    EXPECT_EQ(n, prev_n) << "Num of list unmatch!";

    p = timer_list.get_head();
    while (p){
        std::cin >> input;
        EXPECT_EQ(input, p->expire);
        p = p->next;
    }
    p = timer_list.get_tail();
    while (p){
        std::cin >> input;
        EXPECT_EQ(input, p->expire);
        p = p->prev;
    }
}

TEST(ListTest,timer_init){
    TimerList_check();
}

TEST(ListTest,del_normal){
    UtilTimer* p = hash_map[20];
    hash_map[20] = nullptr;
    timer_list.del_timer(p);
    TimerList_check();
}

TEST(ListTest,del_head){
    UtilTimer* p = hash_map[10];
    hash_map[10] = nullptr;
    timer_list.del_timer(p);
    TimerList_check();
}

TEST(ListTest,del_tail){
    UtilTimer* p = hash_map[50];
    hash_map[50] = nullptr;
    timer_list.del_timer(p);
    TimerList_check();
}

TEST(ListTest,add_timer_normal){
    x = new UtilTimer;
    x->expire = 35;
    hash_map[35] = x;
    timer_list.add_timer(x);
    TimerList_check();
}

TEST(ListTest,add_timer_head){
    x = new UtilTimer;
    x->expire = 5;
    hash_map[5] = x;
    timer_list.add_timer(x);
    TimerList_check();
}

TEST(ListTest,add_timer_tail){
    x = new UtilTimer;
    x->expire = 100;
    timer_list.add_timer(x);
    hash_map[100] = x;
    TimerList_check();
}

TEST(ListTest,adjust_timer_normal){
    x = hash_map[35];
    x->expire = 50;
    hash_map[x->expire] = x;
    hash_map[35] = nullptr;
    timer_list.adjust_timer(x);
    TimerList_check();
}

TEST(ListTest,adjust_timer_head){
    x = hash_map[5];
    x->expire = 20;
    hash_map[5] = nullptr;
    hash_map[x->expire] = x;
    timer_list.adjust_timer(x);
    TimerList_check();
}

int main(int argc,char *argv[])
{
    testing::InitGoogleTest(&argc,argv);
    init();
    return RUN_ALL_TESTS();
}