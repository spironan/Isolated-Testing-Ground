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
        using mutex_type = std::mutex;
        using value_type = T;
        using reference = value_type&;
        using const_reference = value_type const&;
        using size_type = std::size_t;

        void push(value_type const& v);
        void push(value_type&& v);

        std::shared_ptr<T> wait_and_pop();
        std::shared_ptr<T> try_pop();
        
        void clear();

        bool empty() const;
        size_type size() const;

    public:
        threadsafe_stack() = default;
        ~threadsafe_stack() = default;
        threadsafe_stack(threadsafe_stack&& other);
        threadsafe_stack(threadsafe_stack const& other);

        threadsafe_stack operator=(threadsafe_stack const&) =delete;
        threadsafe_stack operator=(threadsafe_stack&&) = delete;

    private:
        void do_push(std::shared_ptr<T> value)
        {
            // Construct a new node.
            std::unique_ptr<node> new_node = std::make_unique<node>();
            // Set the new node's value to the new value.
            new_node->value = value;
            // Lock the top mutex.
            std::lock_guard<mutex_type> lock(top_mutex);
            // Set the new node's next node to the old top node.
            new_node->next = std::move(top_node);
            // Set the new node as the top node.
            top_node = std::move(new_node);
            // Increase the element counter.
            ++count;
        }

        std::shared_ptr<T> do_pop()
        {
            // Extract the value of the top node.
            std::shared_ptr<T> head_data = top_node->value;
            // Point the top node to it's next node.
            top_node = std::move(top_node->next);
            // Decrease the element counter.
            --count;
            // Return the extracted value.
            return head_data;
        }

        void do_copy_construct(threadsafe_stack const& other) requires std::copyable<T>
        {
            // Lock the top mutex.
            std::lock_guard<mutex_type> lock(other.top_mutex);
            // Set the element counter.
            count = other.num_elements.load();
            // Copy All Nodes from other thread recursively
            top_node = other.top_node ? std::move(other.top_node->make_copy()) : nullptr;
        }

    private:

        struct node
        {
            // Value.
            std::shared_ptr<T> value;
            // Next node.
            std::unique_ptr<node> next;

            std::unique_ptr<node> make_copy() const
            {
                std::unique_ptr<node> copied_node = std::make_unique<node>();
                copied_node->value = std::make_shared<T>(*value);
                copied_node->next = next ? std::move(next->make_copy()) : nullptr;
                return copied_node;
            }
        };

        // mutex at the top of the stack
        mutable mutex_type top_mutex;
        // condition variable to notify waiting threads of a push
        std::condition_variable data_cv;
        // top node of the stack
        std::unique_ptr<node> top_node = nullptr;
        // number of elements in stack
        std::atomic_size_t count = 0;
    };

    //
    //Threadsafe stack.
    //
    //Invariants:
    //- top_node == nullptr means that the stack is empty.
    //- Traversing top_node->next will eventually lead to the bottom node.
    //
    //Addtional Requirements:
    //- threadsafe_stack must be able to support type T where T is a non-copyable or non-movable type.
    //- threadsafe_stack does not have to support a non-copyable AND non-movable type.
    //
    //Additional Notes:
    //- threadsafe_stack is non-copyable AND non-movable. (As it has non-copyable and non-movable members.)
    //

    template<typename T>
    inline bool threadsafe_stack<T>::empty() const
    {
        return size() == 0;
    }
    
    template<typename T>
    inline threadsafe_stack<T>::size_type threadsafe_stack<T>::size() const
    {
        return count.load();
    }

    template<typename T>
    inline void threadsafe_stack<T>::push(value_type const& v)
    {
        do_push(std::make_shared<T>(v));
        data_cv.notify_one();
    }
    
    template<typename T>
    inline void threadsafe_stack<T>::push(value_type&& v)
    {
        do_push(std::make_shared<T>(std::forward<T>(v)));
        data_cv.notify_one();
    }
    
    template<typename T>
    inline std::shared_ptr<T> threadsafe_stack<T>::wait_and_pop()
    {
        // lock the top mutex
        std::unique_lock<mutex_type> lock{ top_mutex };
        data_cv.wait(lock, [&]() { return top_node != nullptr; });
        return do_pop();
    }
    
    template<typename T>
    inline std::shared_ptr<T> threadsafe_stack<T>::try_pop()
    {
        // lock the top mutex
        std::unique_lock<mutex_type> lock{ top_mutex };

        if (top_node == nullptr)
            return nullptr;

        return do_pop();
    }

    template<typename T>
    inline void threadsafe_stack<T>::clear()
    {
        std::lock_guard<std::mutex> lock{ top_mutex };
        while (top_node != nullptr)
            do_pop();
    }
    
    template<typename T>
    inline threadsafe_stack<T>::threadsafe_stack(threadsafe_stack&& other)
    {
        do_copy_construct(other);
    }

    template<typename T>
    inline threadsafe_stack<T>::threadsafe_stack(threadsafe_stack const& other)
    {
        do_copy_construct(other);
    }
}
