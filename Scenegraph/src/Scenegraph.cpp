#include "..\include\Scenegraph.h"
#include "..\include\Scenegraph.h"
#include "..\include\Scenegraph.h"
#include "SceneGraph/include/SceneGraph.h"

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

std::vector<scenegraph::value_type> scenegraph::hierarchy_traversal_nodes() const
{
    return hierarchy_traversal_nodes(*this);
}

std::vector<scenegraph::handle_type> scenegraph::hierarchy_traversal_handles() const
{
    return hierarchy_traversal_handles(*this);
}

std::pair<std::vector<scenegraph::value_type>, std::vector<scenegraph::handle_type>> scenegraph::hierarchy_traversal_both() const
{
    return hierarchy_traversal_both(*this);
}

scenegraph::shared_pointer scenegraph::get_root() const
{
    return m_root;
}

std::vector<scenegraph::handle_type> scenegraph::get_root_childs(bool includeItself) const
{
    return get_childs(m_root, includeItself);
}


std::vector<scenegraph::value_type> scenegraph::hierarchy_traversal_nodes(scenegraph const& graph)
{
    shared_pointer root_node = graph.get_root();
    std::stack<scenenode::raw_pointer> s;
    std::vector<scenenode> nodes;
    //std::vector<scenegraph::handle_type const> handles;
    scenenode::raw_pointer curr = root_node.get();
    s.push(curr);
    while (!s.empty())
    {
        s.pop();
        for (auto iter = curr->rbegin(); iter != curr->rend(); ++iter)
        {
            scenenode::shared_pointer child = *iter;
            s.push(child.get());
            nodes.push_back(*child);
            //handles.emplace_back(child->get_handle());
        }
        curr = s.top();
    }

    return nodes;
}

std::vector<scenegraph::handle_type> scenegraph::hierarchy_traversal_handles(scenegraph const& graph)
{
    shared_pointer root_node = graph.get_root();
    std::stack<scenenode::raw_pointer> s;
    //std::vector<scenenode> nodes;
    std::vector<handle_type> handles;
    scenenode::raw_pointer curr = root_node.get();
    s.push(curr);
    while (!s.empty())
    {
        s.pop();
        for (auto iter = curr->rbegin(); iter != curr->rend(); ++iter)
        {
            scenenode::shared_pointer child = *iter;
            s.push(child.get());
            //nodes.push_back(*child);
            handles.emplace_back(child->get_handle());
        }
        curr = s.top();
    }

    return handles;
}

std::pair<std::vector<scenegraph::value_type>, std::vector<scenegraph::handle_type>> scenegraph::hierarchy_traversal_both(scenegraph const& graph)
{
    shared_pointer root_node = graph.get_root();
    std::stack<scenenode::raw_pointer> s;
    std::vector<scenenode> nodes;
    std::vector<scenegraph::handle_type> handles;
    scenenode::raw_pointer curr = root_node.get();
    s.push(curr);
    while (!s.empty())
    {
        s.pop();
        for (auto iter = curr->rbegin(); iter != curr->rend(); ++iter)
        {
            scenenode::shared_pointer child = *iter;
            s.push(child.get());
            nodes.push_back(*child);
            handles.emplace_back(child->get_handle());
        }
        curr = s.top();
    }

    return { nodes, handles };
}
