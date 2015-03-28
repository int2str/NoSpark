#include <cassert>
#include <cstring>
#include <iostream>

#include "../utils/math.h"
#include "../utils/pair.h"
#include "../utils/queue.h"

using std::cout;
using std::endl;

int main()
{
    cout << "Testing utils::min()" << endl;

    {
        assert(utils::min(10, 5) == 5);
        assert(utils::min(1, -1) == -1);
        assert(utils::min(45, 45) == 45);
        assert(utils::min(4, 5) == 4);
        assert(utils::min('A', 'Z') == 'A');
    }
 
    cout << "Testing utils::max()" << endl;

    {
        assert(utils::max(10, 5) == 10);
        assert(utils::max(1, -1) == 1);
        assert(utils::max(45, 45) == 45);
        assert(utils::min(4, 5) == 4);
        assert(utils::max('A', 'Z') == 'Z');
    }

    cout << "Testing utils::Pair()" << endl;

    {
        const char *meaning = "Meaning of life\0";
        utils::Pair<int, const char*> p {42, meaning};
        assert(p.first == 42);
        assert(strcmp(meaning, p.second) == 0);
    }
    
    cout << "Testing utils::queue()" << endl;

    {
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
    }

    // All done!
 
    std::cout << "All tests passed.\n";
}
