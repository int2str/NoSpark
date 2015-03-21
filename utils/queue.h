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
struct Node
{
    T data;
    Node *next;
};

template<typename T>
class Queue
{
public:
    Queue()
        : head(0)
    {
    }

    void push(const T& data)
    {
        if (!head)
        {
            head = new Node<T> {data, 0};

        } else {
            auto n = head;
            while (n->next)
                n = n->next;
            n->next = new Node<T> {data, 0};
        }
    }

    void pop()
    {
        if (head)
        {
            auto n = head;
            head = head->next;
            delete n;
        }
    }

    bool empty() const
    {
        return !head;
    }

    const T& front()
    {
        if (empty())
        {
            const T& e = T();
            return e;
        }
        it = head;
        return head->data;
    }

    bool has_next() const
    {
        return !!it->next;
    }

    const T& next()
    {
        it = it->next;
        return it->data;
    }

private:
    Node<T> *head;
    Node<T> *it;
};

}
