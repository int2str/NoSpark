// NoSpark - OpenEVSE charger firmware
// Copyright (C) 2015 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>.

#pragma once

namespace utils
{

template<typename T>
class Queue
{
    struct Node
    {
        T data;
        Node *next;
    };

public:
    Queue()
        : head(0)
    {
    }

    ~Queue()
    {
        while (!empty())
            pop();
    }

    void push(const T& data)
    {
        if (!head)
        {
            head = new Node {data, 0};

        } else {
            auto n = head;
            while (n->next)
                n = n->next;
            n->next = new Node {data, 0};
        }
    }

    void pop()
    {
        if (empty())
            return;

        auto n = head;
        head = head->next;
        delete n;
    }

    bool empty() const
    {
        return !head;
    }

    const T& front()
    {
        return head->data;
    }

    class const_iterator
    {
        explicit const_iterator(Node *p)
            : it(p)
        {
        }

    public:
        bool operator!= (const const_iterator& rhs)
        {
            return this->it != rhs.it;
        }

        const const_iterator& operator++ ()
        {
            it = it->next;
            return *this;
        }

        const T& operator* ()
        {
            return it->data;
        }

    private:
        friend class Queue;
        Node *it;
    };

    const_iterator begin()
    {
        return const_iterator(head);
    }

    const_iterator end()
    {
        return const_iterator(0);
    }

private:
    Node *head;
};

}
