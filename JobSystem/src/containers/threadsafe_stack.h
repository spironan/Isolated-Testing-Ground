#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

// threadsafe namespace
namespace ts
{
    template <typename T>
    class threadsafe_stack
    {
    public:
        using value_type = T;
        using return_type = std::shared_ptr<T>;
        using reference = value_type&;
        using const_reference = value_type const&;
        using size_type = int;

        threadsafe_stack() = default;
        ~threadsafe_stack() = default;
        threadsafe_stack(threadsafe_stack&& other)
        {
            do_copy_construct(other);
        }

        threadsafe_stack(threadsafe_stack const& other)
        {
            do_copy_construct(other);
        }

        threadsafe_stack operator=(threadsafe_stack const&) =delete;
        threadsafe_stack operator=(threadsafe_stack&&) = delete;

        reference top()
        {
            return top_node;
        }

        const_reference top() const
        {
            return top_node;
        }

        bool empty() const
        {
            return size() == 0;
        }

        size_type size() const
        {
            return num_elems.load();
        }

        void push(value_type const& v)
        {
            do_push(std::make_shared<T>(v));
            cv.notify_one();
        }
        
        void emplace(value_type && v)
        {
            do_push(std::make_shared<T>(std::forward<T>(v)));
            cv.notify_one();
        }
        
        return_type  wait_and_pop()
        {
            // lock the top mutex
            std::unique_lock<mutex_type> lock{ top_mutex };
            cv.wait(lock, [&]() { return top_node != nullptr; });
            return do_pop();
        }

        return_type  try_pop()
        {
            // lock the top mutex
            std::unique_lock<mutex_type> lock{ top_mutex };
            if (top_node == nullptr) 
                return nullptr;

            return do_pop();
        }

    private:
        void do_push(std::shared_ptr<T> _value)
        {
            // Construct a new node.
            std::unique_ptr<node> new_node = std::make_unique<node>();
            // Set the new node's value to the new value.
            new_node->value = _value;
            // Lock the top mutex.
            std::lock_guard<mutex_type> lock(top_mutex);
            // Set the new node's next node to the old top node.
            new_node->next = std::move(top_node);
            // Set the new node as the top node.
            top_node = std::move(new_node);
            // Increase the element counter.
            ++num_elems;
        }

        std::shared_ptr<T> do_pop()
        {
            // Extract the value of the top node.
            std::shared_ptr<T> head_data = top_node->value;
            // Point the top node to it's next node.
            top_node = std::move(top_node->next);
            // Decrease the element counter.
            --num_elems;
            // Return the extracted value.
            return head_data;
        }

        void do_copy_construct(threadsafe_stack const& _threadsafe_stack) requires std::copyable<T>
        {
            // Lock the top mutex.
            std::lock_guard<mutex_type> lock(_threadsafe_stack.top_mutex);
            // Set the element counter.
            num_elems = _threadsafe_stack.num_elements.load();
            // Copy All Nodes from other thread recursively
            top_node = _threadsafe_stack.top_node ? std::move(_threadsafe_stack.top_node->copy()) : nullptr;
        }

    private:
        using mutex_type = std::timed_mutex;

        struct node
        {
            // Value.
            std::shared_ptr<T> value;
            // Next node.
            std::unique_ptr<node> next;

            std::unique_ptr<node> copy() const
            {
                std::unique_ptr<node> copied_node = std::make_unique<node>();
                copied_node->value_ = std::make_shared<T>(*value);
                copied_node->next_ = next ? std::move(next->copy()) : nullptr;
                return copied_node;
            }
        };

        // mutex at the top of the stack
        mutable mutex_type top_mutex;
        // condition variable to notify waiting threads of a push
        std::condition_variable_any cv;
        // top node of the stack
        std::unique_ptr<node> top_node = nullptr;
        // number of elements in stack
        std::atomic_size_t num_elems = 0;
    };
}
