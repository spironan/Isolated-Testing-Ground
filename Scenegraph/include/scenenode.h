#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>

class scenenode : public std::enable_shared_from_this<scenenode>
{
public:
    using handle_type       = std::uint64_t;
    using value_type        = scenenode;
    using raw_pointer       = value_type*;
    using const_raw_pointer = value_type const*;
    using shared_pointer    = std::shared_ptr<value_type>;
    using weak_pointer      = std::weak_ptr<value_type>;

    static constexpr handle_type NOTFOUND = static_cast<handle_type>(-1);

public:
    scenenode() = delete;
    scenenode(std::string_view name, handle_type handle);
    scenenode(scenenode const& other) = default;
    ~scenenode() = default;

    auto operator<=>(scenenode const&) const = default;

    void detach();
    bool add_child(shared_pointer node);

    bool contains(shared_pointer node) const;
    std::size_t get_direct_child_count() const;
    std::size_t get_total_child_count() const;
    handle_type get_handle() const;
    handle_type get_parent_handle() const;
    weak_pointer get_parent() const;
    std::vector<handle_type> get_all_child_handles(bool includeItself = false) const;
    std::vector<handle_type> get_direct_child_handles(bool includeItself = false) const;
    void print_recursive() const;

    std::vector<shared_pointer> get_direct_child() const { return m_childs; }
    
    std::vector<shared_pointer>::iterator           begin()  { return m_childs.begin(); }
    std::vector<shared_pointer>::iterator           end()    { return m_childs.end(); }
    std::vector<shared_pointer>::reverse_iterator   rbegin() { return m_childs.rbegin(); }
    std::vector<shared_pointer>::reverse_iterator   rend()   { return m_childs.rend(); }

    bool move_to(shared_pointer sibling_node, bool next = false);
    bool move_to_after(shared_pointer sibling_node);

    void set_debug_name(std::string_view new_name);

private:
    // contains handle to actual data in scene-graph hierarchy
    weak_pointer m_parent = {};
    std::vector<shared_pointer> m_childs = {};
    std::string m_debugName = "default name";
    handle_type m_handle = NOTFOUND;

private:
    void print_recursive(size_t depth, bool printParent = true, bool printChilds = true) const;
    void remove(shared_pointer node);

    void rearrange_childs(shared_pointer src_child, shared_pointer target_child, bool next = false);
};

void PrintSceneNode(scenenode::const_raw_pointer node);

static constexpr std::size_t scenenode_size = sizeof(scenenode); //112 + 16