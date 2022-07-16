#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include <stack>
#include "scenenode.h"

class scenegraph
{
public:
    using handle_type       = scenenode::handle_type;
    using value_type        = scenenode::value_type;
    using raw_pointer       = scenenode::raw_pointer;
    using shared_pointer    = scenenode::shared_pointer;
    using const_raw_pointer = scenenode::const_raw_pointer;
    using weak_pointer      = scenenode::weak_pointer;

    static constexpr handle_type ROOTID = std::numeric_limits<handle_type>::min();

private:
    shared_pointer m_root = nullptr;
    
public:
    //static functions
    static void add_child(shared_pointer parent, shared_pointer child);
    static std::vector<handle_type> get_childs(shared_pointer target, bool includeItself = false);
    static std::vector<value_type> hierarchy_traversal_nodes(scenegraph const& graph);
    static std::vector<handle_type> hierarchy_traversal_handles(scenegraph const& graph);
    static std::pair<std::vector<value_type>, std::vector<handle_type>> hierarchy_traversal_both(scenegraph const& graph);

public:
    scenegraph() = delete;
    scenegraph(std::string_view name);
    scenegraph(scenegraph const& other) = default;
    scenegraph(scenegraph && other) = default;
    scenegraph& operator=(scenegraph const& other) = default;
    scenegraph& operator=(scenegraph && other) = default;
    ~scenegraph() = default;

public:
    void print() const;
    shared_pointer get_root() const;
    std::vector<handle_type> get_root_childs(bool includeItself = false) const;

    shared_pointer create_new_child(std::string_view childName, handle_type unique_id);
    void add_child(shared_pointer child);
    
public:
    // Traversal methods
    std::vector<value_type> hierarchy_traversal_nodes() const;
    std::vector<handle_type> hierarchy_traversal_handles() const;
    std::pair<std::vector<value_type>, std::vector<handle_type>> hierarchy_traversal_both() const;

    /*SceneNode::handle_type GetParent(SceneNode const* target) const
    {
        return GetData(target->getParentHandle());
    }

    SceneNode::handle_type GetData(SceneNode const* target) const
    {
        assert(target == nullptr);
        assert(m_data.contains(target->getHandle()) == false);

        return m_data.at(target->getHandle());
    }

    SceneNode::handle_type GetData(Key handle) const
    {
        return m_data.at(handle);
    }*/
};

static constexpr std::size_t sizeof_scenegraph = sizeof(scenegraph); // 16 + 112 = 128


