#include <cassert>
#include <cstring>
#include <iostream>

#include "../utils/math.h"
#include "../utils/movingaverage.h"
#include "../utils/pair.h"
#include "../utils/queue.h"

using std::cout;
using std::endl;

using namespace nospark;

void test_utils_min()
{
    cout << "Testing utils::min()" << endl;

    assert(utils::min(10, 5) == 5);
    assert(utils::min(1, -1) == -1);
    assert(utils::min(45, 45) == 45);
    assert(utils::min(4, 5) == 4);
    assert(utils::min('A', 'Z') == 'A');
}

void test_utils_max()
{
    cout << "Testing utils::max()" << endl;

    assert(utils::max(10, 5) == 10);
    assert(utils::max(1, -1) == 1);
    assert(utils::max(45, 45) == 45);
    assert(utils::min(4, 5) == 4);
    assert(utils::max('A', 'Z') == 'Z');
}

void test_utils_square_root()
{
    cout << "Testing utils::square_root()" << endl;

    assert((utils::square_root(81) == 9));
    assert((utils::square_root(144) == 12));
    assert((utils::square_root(150) == 12));
    assert((utils::square_root(1) == 1));
    assert((utils::square_root(0) == 0));
}

void test_utils_pair()
{
    cout << "Testing utils::Pair()" << endl;

    const char *meaning = "Meaning of life\0";
    utils::Pair<int, const char*> p {42, meaning};
    assert(p.first == 42);
    assert(strcmp(meaning, p.second) == 0);
}

void test_utils_queue()
{
    cout << "Testing utils::Queue()" << endl;

    utils::Queue<int> q;
    assert(q.empty());

    q.push(1);
    q.push(2);
    q.push(3);

    auto i = 1;
    for (auto it = q.begin(); it != q.end(); ++it)
        assert(*it == i++);

    assert(!q.empty());
    assert(q.front() == 1);

    q.pop();

    assert(!q.empty());
    assert(q.front() == 2);

    q.pop();

    assert(!q.empty());
    assert(q.front() == 3);

    q.pop();
    assert(q.empty());

    q.push(2);
    q.push(1);
    q.push(2);
    q.push(2);
    q.push(3);
    q.push(2);

    for (auto it = q.begin(); it != q.end();)
    {
        if (*it == 2)
            q.erase(it++);
        else
            ++it;
    }

    assert(q.front() == 1);

    q.pop();
    assert(q.front() == 3);

    q.pop();
    assert(q.empty());
}

void test_utils_movingaverage()
{
    cout << "Testing utils::MovingAverage()" << endl;

    utils::MovingAverage<int, 3> ma;

    assert(ma.get() == 0);

    ma.push(3);
    ma.push(1);
    assert(ma.get() == 2);

    ma.push(8);
    assert(ma.get() == 4);

    ma.push(3);
    assert(ma.get() == 4);

    ma.clear();
    assert(ma.get() == 0);

    ma.push(3);
    ma.push(11);
    assert(ma.get() == 7);
}

int main()
{
    test_utils_min();
    test_utils_max();
    test_utils_square_root();
    test_utils_movingaverage();
    test_utils_pair();
    test_utils_queue();

    std::cout << "All tests passed.\n";
}
