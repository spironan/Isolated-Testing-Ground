#include "SceneGraph.h"

void scenegraph::add_child(shared_pointer parent, shared_pointer child)
{
    parent->add_child(child);
}

std::vector<scenegraph::handle_type> scenegraph::get_childs(shared_pointer target, bool includeItself)
{
    std::vector<handle_type> vec;
    if (includeItself)
    {
        vec.emplace_back(target->get_handle());
    }

    std::vector<handle_type> childHandles = target->get_all_child_handles();
    vec.insert(vec.end(), childHandles.begin(), childHandles.end());

    return vec;
}


scenegraph::scenegraph(std::string_view name)
    : m_root{ std::make_shared<value_type>(name, ROOTID) }
{
}

scenegraph::shared_pointer scenegraph::create_new_child(std::string_view childName, handle_type unique_id)
{
    scenenode::shared_pointer node = std::make_shared<scenenode>(childName, unique_id);
    m_root->add_child(node);
    return node;
}

void scenegraph::print() const
{
    m_root->print_recursive();
}

void scenegraph::add_child(shared_pointer child)
{
    m_root->add_child(child);
}

scenegraph::shared_pointer scenegraph::get_root() const
{
    return m_root;
}

std::vector<scenegraph::handle_type> scenegraph::get_root_childs(bool includeItself) const
{
    return get_childs(m_root, includeItself);
}
