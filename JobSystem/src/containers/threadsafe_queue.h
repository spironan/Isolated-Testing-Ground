#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>

namespace ts
{
    template<typename T>
    class threadsafe_queue
    {
    public:
        using size_type = std::size_t;

        std::shared_ptr<T> try_pop();
        bool try_pop(T& value);
        std::shared_ptr<T> wait_and_pop();
        void wait_and_pop(T& value);
        void push(T new_value);
        void clear();

        bool empty() const;
        size_type size() const;

    public:

        // initialize with dummy node is intentional.
        threadsafe_queue() 
            : head{std::make_unique<node>()}
            , tail{head.get()}
        {
        }
        ~threadsafe_queue() = default;

        threadsafe_queue(threadsafe_queue const&) = delete;
        threadsafe_queue& operator=(threadsafe_queue const&) = delete;
        
        threadsafe_queue(threadsafe_queue &&) = delete;
        threadsafe_queue& operator=(threadsafe_queue &&) = delete;

    private:
        struct node
        {
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
        };
        std::mutex head_mutex;
        std::unique_ptr<node> head;
        std::mutex tail_mutex;
        node* tail;
        std::condition_variable data_cv;
        std::atomic_size_t count;

    private:

        node const* get_tail() const
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }

        node* get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }

        // modifies the list to remove the head item
        std::unique_ptr<node> pop_head()
        {
            std::unique_ptr<node> old_head = std::move(head);
            head = std::move(old_head->next);
            --count;
            return old_head;
        }
        
        //// waits for the queue to have some data to pop
        //// this function ensures the same lock is held while the data 
        //// is modified by the relevant wait_pop_head()
        //std::unique_lock<std::mutex> wait_for_data(std::stop_token stop_token)
        //{
        //    std::unique_lock<std::mutex> head_lock{ head_mutex };
        //    if (data_cv.wait(head_lock, stop_token, [&]() { return head.get() != get_tail(); }) == false)
        //        return;

        //    return std::move(head_lock);
        //}

        // waits for the queue to have some data to pop
        // this function ensures the same lock is held while the data 
        // is modified by the relevant wait_pop_head()
        std::unique_lock<std::mutex> wait_for_data()
        {
            std::unique_lock<std::mutex> head_lock{ head_mutex };
            data_cv.wait(head_lock, [&]() { return head.get() != get_tail(); });
            return std::move(head_lock);
        }

        std::unique_ptr<node> wait_pop_head()
        {
            std::unique_lock<std::mutex> head_lock{ wait_for_data() };
            return pop_head();
        }

        std::unique_ptr<node> wait_pop_head(T& value)
        {
            std::unique_lock<std::mutex> head_lock{ wait_for_data() };
            value = std::move(*head->data);
            return pop_head();
        }

        std::unique_ptr<node> try_pop_head()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if (head.get() == get_tail())
            {
                return nullptr;
            }
            return pop_head();
        }
        
        std::unique_ptr<node> try_pop_head(T& value)
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if (head.get() == get_tail())
            {
                return nullptr;
            }
            value = std::move(*head->data);
            return pop_head();
        }
    };
    
    //
    //Threadsafe queue implementation without std::queue.
    //
    //Invariants:
    //- tail->value_ == nullptr
    //- tail->next_ == nullptr
    //- head == tail_ implies an empty list.
    //- A single element list has head_->next == tail.
    //- For each node x in the queue, where x != tail, x->value points to an instance of T and x->next points to the next node in the queue.
    //- x->next == tail implies x is the last node in the queue.
    //- Traversing head->next will eventually lead to tail.
    //
    //Addtional Requirements:
    //- threadsafe_queue must be able to support type T where T is a non-copyable or non-movable type.
    //- threadsafe_queue does not have to support a non-copyable AND non-movable type.
    //
    //Additional Notes:
    //- threadsafe_queue is non-copyable AND non-movable. (As it has non-copyable and non-movable members.)
    

    template<typename T>
    inline std::shared_ptr<T> threadsafe_queue<T>::try_pop()
    {
        std::unique_ptr<node> old_head = try_pop_head();
        return old_head ? old_head->data : nullptr;
    }

    template<typename T>
    inline bool threadsafe_queue<T>::try_pop(T& value)
    {
        std::unique_ptr<node> const old_head = try_pop_head(value);
        return old_head != nullptr;
    }

    template<typename T>
    inline std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop()
    {
        std::unique_ptr<node> const old_head = wait_pop_head();
        return old_head->data;
    }

    template<typename T>
    inline void threadsafe_queue<T>::wait_and_pop(T& value)
    {
        std::unique_ptr<node> const old_head = wait_pop_head(value);
    }

    template<typename T>
    inline void threadsafe_queue<T>::push(T new_value)
    {
        std::shared_ptr<T> new_data = std::make_shared<T>(std::move(new_value));
        std::unique_ptr<node> dummy = std::make_unique<node>();
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;
            node* const new_tail = dummy.get();
            tail->next = std::move(dummy);
            tail = new_tail;
            ++count;
        }
        data_cv.notify_one();
    }

    template<typename T>
    inline bool threadsafe_queue<T>::empty() const
    {
        return size() == 0;
    }

    template<typename T>
    inline threadsafe_queue<T>::size_type threadsafe_queue<T>::size() const
    {
        return count.load();
    }

    //template<typename T>
    //inline bool threadsafe_queue<T>::empty()
    //{
    //    //std::lock_guard<std::mutex> head_lock(head_mutex);
    //    //return (head.get() == get_tail());
    //}

    template<typename T>
    inline void threadsafe_queue<T>::clear()
    {
        // Lock the mutexes to prevent anyone from pushing
        std::lock_guard<std::mutex> head_lock{ head_mutex };
        std::lock_guard<std::mutex> tail_lock{ tail_mutex };
        while (head.get() != tail)
            pop_head();
    }

}
