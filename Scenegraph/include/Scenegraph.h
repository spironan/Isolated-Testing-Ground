#pragma once

#include <memory>
#include <vector>
#include <cassert>
#include "scenenode.h"

class scenegraph
{
public:
    using handle_type       = scenenode::handle_type;
    using value_type        = scenenode::value_type;
    using raw_pointer       = scenenode::raw_pointer;
    using shared_pointer    = scenenode::shared_pointer;
    using const_raw_pointer = scenenode::const_raw_pointer;

    static constexpr handle_type ROOTID = std::numeric_limits<handle_type>::min();

private:
    shared_pointer m_root = nullptr;
    
public:
    //static functions

    static void add_child(raw_pointer parent, raw_pointer child);
    static std::vector<handle_type> get_childs(const_raw_pointer target, bool includeItself = false);

public:

    scenegraph() = default;
    scenegraph(std::string_view name);

    void print() const;
    shared_pointer get_root() const;
    std::vector<handle_type> get_root_childs(bool includeItself = false) const;

    shared_pointer create_new_child(std::string_view childName, handle_type unique_id);
    void add_child(raw_pointer child);

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
