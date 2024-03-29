#include "scenegraph/include/scenenode.h"

scenenode::scenenode(std::string_view name, handle_type handle)
    : m_debugName{ name }
    , m_handle{ handle }
    , m_parent{}
    , m_childs{}
{}

void scenenode::print_recursive(size_t depth, bool printParent, bool printChilds) const
{
    std::cout << "(" << depth << ") - " << m_debugName << " [" << m_handle << "]";

    if (printParent)
    {
        std::cout << " / ParentID: {" << get_parent_handle() << "}";
    }

    if (printChilds)
    {
        std::cout << " / ChildIDs: {";
        for (auto& childId : get_all_child_handles())
            std::cout << childId << ", ";
        std::cout << "}";
    }

    std::cout << "\n";
    for (auto& child : m_childs)
    {
        std::cout << "\t";
        for (size_t i = 0; i < depth; ++i)
            std::cout << "\t";

        child->print_recursive(depth + 1);
    }
}

void scenenode::remove(shared_pointer node)
{
    m_childs.erase(std::remove(m_childs.begin(), m_childs.end(), node), m_childs.end());
}

void scenenode::rearrange_childs(shared_pointer src_child, shared_pointer target_child, bool next)
{
    // remove the child
    remove(src_child);
    
    auto target_position = std::find(m_childs.begin(), m_childs.end(), target_child);
    if (target_position != m_childs.end())
    {
        if (next)
            target_position++;

        // insert at that location
        m_childs.emplace(target_position, src_child);
    }
    else
    {
        m_childs.emplace_back(src_child);
    }
}

bool scenenode::has_child() const
{
    return m_childs.size() > 0;
}

bool scenenode::contains(shared_pointer node) const
{
    if (node == nullptr)
        return false;

    for (auto& child : m_childs)
    {
        if (child == node || child->contains(node))
            return true;
    }

    return false;
}

void scenenode::detach()
{
    if (scenenode::shared_pointer parent = m_parent.lock())
    {
        parent->remove(shared_from_this());
        m_parent.reset();
        //reset our depth
        m_depth = 0;
    }
}

bool scenenode::add_child(shared_pointer node)
{
    // ensure target is eligible.
    // ensure parent is not already target and target's child dont contain parent
    if (node == shared_from_this() ||
        node->m_parent.lock() == shared_from_this() ||
        node->contains(shared_from_this()))
        return false;

    // if attached to another parent
    if (scenenode::shared_pointer parent = node->m_parent.lock())
    {
        parent->remove(node);
    }

    // performing re-pointing
    node->m_parent = shared_from_this();
    m_childs.emplace_back(node);
    node->m_depth = m_depth + 1;

    return true;
}

size_t scenenode::get_direct_child_count() const
{
    return m_childs.size();
}

size_t scenenode::get_total_child_count() const
{
    size_t count = m_childs.size();

    for (auto& child : m_childs)
        count += child->get_total_child_count();

    return count;
}

scenenode::handle_type scenenode::get_handle() const
{
    return m_handle;
}

scenenode::handle_type scenenode::get_parent_handle() const
{
    return m_parent.expired() ? NOTFOUND : m_parent.lock()->m_handle;
}

scenenode::weak_pointer scenenode::get_parent() const
{
    return m_parent;
}

std::uint32_t scenenode::get_depth() const
{
    return m_depth;
}

std::vector<scenenode::handle_type> scenenode::get_all_child_handles(bool includeItself) const
{
    std::vector<handle_type> handles;

    if (includeItself)
    {
        handles.emplace_back(m_handle);
    }

    for (auto& child : m_childs)
    {
        handles.emplace_back(child->m_handle);

        for (auto& grandchilds : child->get_all_child_handles())
        {
            handles.emplace_back(grandchilds);
        }
    }

    return handles;
}

std::vector<scenenode::handle_type> scenenode::get_direct_child_handles(bool includeItself) const
{
    std::vector<handle_type> handles;

    if (includeItself)
    {
        handles.emplace_back(m_handle);
    }

    for (auto& child : m_childs)
    {
        handles.emplace_back(child->m_handle);
    }

    return handles;
}

void scenenode::print_recursive() const
{
    print_recursive(0);
}

bool scenenode::move_to(shared_pointer sibling_node, bool next)
{
    //make sure they are not the same node
    if (sibling_node == shared_from_this())
        return false;

    // make sure they share the same parent
    auto parent = get_parent().lock();
    bool verified_sibling = parent->contains(sibling_node);
    
    if (verified_sibling)
    {
        parent->rearrange_childs(shared_from_this(), sibling_node, next);
        return true;
    }

    return false;
}

bool scenenode::move_to_after(shared_pointer sibling_node)
{
    return move_to(sibling_node, true);
}

void scenenode::set_debug_name(std::string_view new_name)
{
    m_debugName = new_name;
}

void PrintSceneNode(scenenode::const_raw_pointer node)
{
    // "Computation" with raw_pointer-to-const
    scenenode::const_raw_pointer root_ptr = node;
    root_ptr->print_recursive();
    size_t directChilds = root_ptr->get_direct_child_count();
    size_t totalChilds = root_ptr->get_total_child_count();

    std::cout << "direct childs : " << directChilds << std::endl;
    std::cout << "total childs : " << totalChilds << std::endl;
}
