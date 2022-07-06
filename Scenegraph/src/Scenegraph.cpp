#include "Scenegraph.h"

SceneNode::SceneNode(std::string const& name, handle_type handle)
    : m_debugName{ name }
    , m_handle{ handle }
{}

void SceneNode::printRecursive(size_t depth, bool printParent, bool printChilds) const
{
    std::cout << "(" << depth << ") - " << m_debugName << " [" << m_handle << "]";

    if (printParent)
    {
        std::cout << " / ParentID: {" << getParentHandle() << "}";
    }

    if (printChilds)
    {
        std::cout << " / ChildIDs: {";
        for (auto& childId : getAllChildHandles())
            std::cout << childId << ", ";
        std::cout << "}";
    }

    std::cout << "\n";
    for (auto& child : m_childs)
    {
        std::cout << "\t";
        for (size_t i = 0; i < depth; ++i)
            std::cout << "\t";

        child->printRecursive(depth + 1);
    }
}

void SceneNode::remove(SceneNode* node)
{
    m_childs.erase(std::remove(m_childs.begin(), m_childs.end(), node), m_childs.end());
}

bool SceneNode::contains(SceneNode* node) const
{
    if (node == nullptr)
        return false;

    for (auto* child : m_childs)
    {
        if (child->contains(node) || *child == *node)
            return true;
    }

    return false;
}

void SceneNode::detach()
{
    if (m_parent != nullptr)
    {
        m_parent->remove(this);
        m_parent = nullptr;
    }
}

void SceneNode::addChild(SceneNode* node)
{
    //ensure target is eligible.
    // ensure parent is not already target and target's child dont contain parent
    if (node == this ||
        node->m_parent == this ||
        node->contains(this))
        return;

    // if attached to another parent
    if (node->m_parent != nullptr)
    {
        node->m_parent->remove(node);
    }

    // performing re-pointing
    node->m_parent = this;
    m_childs.emplace_back(node);
}

size_t SceneNode::getDirectChildCount() const
{
    return m_childs.size();
}

size_t SceneNode::getTotalChildCount() const
{
    size_t count = m_childs.size();

    for (auto& child : m_childs)
        count += child->getTotalChildCount();

    return count;
}

SceneNode::handle_type SceneNode::getHandle() const
{
    return m_handle;
}

SceneNode::handle_type SceneNode::getParentHandle() const
{
    return m_parent ? m_parent->m_handle : NOTFOUND;
}

std::vector<SceneNode::handle_type> SceneNode::getAllChildHandles() const
{
    std::vector<handle_type> handles;

    for (auto& child : m_childs)
    {
        handles.emplace_back(child->m_handle);

        for (auto& grandchilds : child->getAllChildHandles())
        {
            handles.emplace_back(grandchilds);
        }
    }

    return handles;
}

void SceneNode::printRecursive() const
{
    printRecursive(0);
}

void PrintNode(SceneNode const* node)
{
    // "Computation" with pointer-to-const
    const SceneNode* root_ptr = node;
    root_ptr->printRecursive();
    size_t directChilds = root_ptr->getDirectChildCount();
    size_t totalChilds = root_ptr->getTotalChildCount();

    std::cout << "direct childs : " << directChilds << std::endl;
    std::cout << "total childs : " << totalChilds << std::endl;
}
