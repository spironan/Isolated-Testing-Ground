#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>

class scenenode
{
public:
    using handle_type       = size_t;
    using value_type        = scenenode;
    using raw_pointer       = value_type*;
    using const_raw_pointer = value_type const*;
    using shared_pointer    = std::shared_ptr<value_type>;

    static constexpr handle_type NOTFOUND = static_cast<handle_type>(-1);

public:
    scenenode(std::string_view name, handle_type handle);
    scenenode(scenenode const& other);

    auto operator<=>(scenenode const&) const = default;

    void detach();
    void add_child(raw_pointer node);

    bool contains(raw_pointer node) const;
    size_t get_direct_child_count() const;
    size_t get_total_child_count() const;
    handle_type get_handle() const;
    handle_type get_parent_handle() const;
    const_raw_pointer get_parent() const;
    std::vector<handle_type> get_all_child_handles() const;
    void print_recursive() const;

private:
    // contains handle to actual data in scene-graph hierarchy
    raw_pointer m_parent = nullptr;
    std::vector<raw_pointer> m_childs = {};
    std::string m_debugName = "default name";
    handle_type m_handle = NOTFOUND;

private:
    void print_recursive(size_t depth, bool printParent = true, bool printChilds = true) const;
    void remove(raw_pointer node);

};

void PrintSceneNode(scenenode::const_raw_pointer node);
