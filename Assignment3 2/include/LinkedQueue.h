#ifndef SDP_LINKED_QUEUE_H
#define SDP_LINKED_QUEUE_H

#include <cstddef>
#include <memory>
#include <utility>

#include "Errors.h"

namespace sdp::ds
{

    template <typename T>
    class LinkedQueue
    {
    private:
        struct Node
        {
            T value;
            std::unique_ptr<Node> next;

            explicit Node(T data)
                : value(std::move(data)), next(nullptr) {}
        };

        std::unique_ptr<Node> head;
        Node *tail = nullptr;
        std::size_t count = 0;

    public:
        LinkedQueue() = default;
        LinkedQueue(const LinkedQueue &) = delete;
        LinkedQueue &operator=(const LinkedQueue &) = delete;
        LinkedQueue(LinkedQueue &&) = delete;
        LinkedQueue &operator=(LinkedQueue &&) = delete;

        ~LinkedQueue()
        {
            // Huy tung nut, tranh de quy sau voi hang doi lon.
            while (head)
            {
                auto node = std::move(head);
                head = std::move(node->next);
            }
        }
        bool empty() const
        {
            return count == 0;
        }

        std::size_t size() const
        {
            return count;
        }

        void push(T value)
        {
            auto newNode = std::make_unique<Node>(std::move(value));

            Node *newTail = newNode.get();

            if (empty())
            {
                head = std::move(newNode);
            }
            else
            {
                tail->next = std::move(newNode);
            }

            tail = newTail;
            ++count;
        }

        const T &front() const
        {
            if (empty())
            {
                throw sdp::EmptyContainerError("Hang doi dang rong");
            }

            return head->value;
        }

        T pop()
        {
            if (empty())
            {
                throw sdp::EmptyContainerError("Khong the pop hang doi rong");
            }

            T result = std::move(head->value);

            auto oldHead = std::move(head);
            head = std::move(oldHead->next);

            --count;

            if (empty())
            {
                tail = nullptr;
            }

            return result;
        }
    };

} // namespace sdp::ds

#endif
