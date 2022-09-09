#pragma once

#include <memory>
#include <vector>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <functional>   // for std::hash
#include <map>

namespace ts
{
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class threadsafe_map
    {
    public:
        using key_type      = Key;
        using value_type    = Value;
        using hash_type     = Hash;
        using size_type     = std::size_t;

        Value at(Key const& key, Value const& default_value = Value{}) const;
        void insert_or_update(Key const& key, Value const& value);
        void erase(Key const& key);
        std::map<Key, Value> get_map() const;

        threadsafe_map(size_type bucket_count = 19, hash_type const& hasher = Hash())
            : buckets { bucket_count }
            , hasher{ hasher }
        {
            for (size_type i = 0; i < bucket_count; ++i)
            {
                buckets[i].reset(std::make_unique<bucket>());
            }
        }
        ~threadsafe_map() = default;

        threadsafe_map(threadsafe_lookup_table const& other) = delete;
        threadsafe_map& operator=(threadsafe_map const& other) = delete;

    private:
        class bucket
        {
        public:
            using value_type = std::pair<Key, Value>;
            using container_type = std::list<value_type>;
            using iterator = typename value_type::iterator;
            using mutex_type = std::shared_mutex;
        
        private:
            container_type container;
            mutable mutex_type mutex;

            iterator find_entry_for(Key const& key) const
            {
                return std::find_if(container.begin(), container.end(), 
                    [&](value_type const& item) 
                    { 
                        return item.first == key; 
                    });
            }

        public:
            
            Value at(Key const& key, Value const& default_value) const
            {
                std::shared_lock<std::shared_mutex> lock{ mutex };
                iterator const found_entry = find_entry_for(key);
                return (found_entry == container.end()) ? default_value : found_entry->second;
            }

            void insert_or_update(Key const& key, Value const& value)
            {
                std::unique_lock<std::shared_mutex> lock { mutex };
                iterator const found_entry = find_entry_for(key);
                if (found_entry == container.end())
                {
                    container.push_back(value_type(key, value));
                }
                else
                {
                    found_entry->second = value;
                }
            }
            
            void erase(Key const& key)
            {
                std::unique_lock<std::shared_mutex> lock{ mutex };
                iterator const found_entry = find_entry_for(key);
                if (found_entry != container.end())
                {
                    container.erase(found_entry);
                }
            }
        };

    private:
        std::vector<std::unique_ptr<bucket>> buckets;
        Hash hasher;
    
    private:
        bucket& get_bucket(Key const& key) const
        {
            size_type const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }
    };

    template<typename Key, typename Value, typename Hash>
    inline Value threadsafe_map<Key, Value, Hash>::at(Key const& key, Value const& default_value) const
    {
        return get_bucket(key).at(key, default_value);
    }

    template<typename Key, typename Value, typename Hash>
    inline void threadsafe_map<Key, Value, Hash>::insert_or_update(Key const& key, Value const& value)
    {
        get_bucket(key).insert_or_update(key, value);
    }

    template<typename Key, typename Value, typename Hash>
    inline void threadsafe_map<Key, Value, Hash>::erase(Key const& key)
    {
        get_bucket(key).erase(key);
    }

    template<typename Key, typename Value, typename Hash>
    inline std::map<Key, Value> threadsafe_map<Key, Value, Hash>::get_map() const
    {
        std::vector<std::unique_lock<std::shared_mutex> > locks;
        for (size_type i = 0; i < buckets.size(); ++i)
        {
            locks.push_back(std::unique_lock<std::shared_mutex>(buckets[i].mutex));
        }
        std::map<Key, Value> res;
        for (size_type i = 0; i < buckets.size(); ++i)
        {
            for (auto& elem : buckets[i].container)
            {
                res.insert(elem);
            }

            /*for (bucket::iterator it = buckets[i].container.begin();
            it != buckets[i].container.end();
            ++it)
            {
            res.insert(*it);
            }*/
        }
        return res;
    }
}
