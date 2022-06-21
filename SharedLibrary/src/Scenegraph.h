#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "UUID.h"
#include <unordered_map>
#include <cassert>

class SceneNode
{
public:
    using handle_type = size_t;
    static constexpr handle_type NOTFOUND = static_cast<handle_type>(-1);

private:
    // contains handle to actual data in scene-graph hierarchy
    SceneNode* m_parent = nullptr;
    std::vector<SceneNode*> m_childs = {};
    std::string m_debugName = "default name";
    handle_type m_handle = NOTFOUND;

    void printRecursive(size_t depth, bool printParent = true, bool printChilds = true) const;
    void remove(SceneNode* node);

public:
    SceneNode(std::string const& name, handle_type handle);

    SceneNode(SceneNode const& other)
        : m_parent { other.m_parent }
        , m_childs { other.m_childs }
        , m_debugName { other.m_debugName }
        , m_handle { other.m_handle }
    {
    }

    auto operator<=>(SceneNode const&) const = default;

    bool contains(SceneNode* node) const;

    void detach();

    void addChild(SceneNode* node);

    size_t getDirectChildCount() const;

    size_t getTotalChildCount() const;

    handle_type getHandle() const;

    handle_type getParentHandle() const;

    std::vector<handle_type> getAllChildHandles() const;

    void printRecursive() const;
};

void PrintNode(SceneNode const* node);

// System will have one scenegraph object at any point in time.
// Data must be default constructible
//template<typename Data, typename = std::enable_if_t<std::is_default_constructible_v<Data>> >

struct Scenegraph
{
public:
    using Key = oo::UUID;

private:
    SceneNode m_root;
    //std::unordered_map<Key, Data> m_data;

public:
    Scenegraph(std::string const& name) : m_root{ name, 0 } { /*m_data.emplace(m_root.getHandle(), Data{});*/ }

    void Print() const
    {
        m_root.printRecursive();
    }

    void AddChildToRoot(SceneNode* child/*, Data const& data*/)
    {
        m_root.addChild(child);
        /*m_data[child->getHandle()] = data;*/
    }

    void AddChild(SceneNode* parent, SceneNode* child)
    {
        parent->addChild(child);
    }

    SceneNode GetRoot() const
    {
        return m_root;
    }

    std::vector<SceneNode::handle_type> GetChilds(SceneNode const* target, bool includeItself = false) const
    {
        std::vector<SceneNode::handle_type> vec;
        if (includeItself)
        {
            vec.emplace_back(target->getHandle());
        }
        
        std::vector<SceneNode::handle_type> childHandles = target->getAllChildHandles();
        vec.insert(vec.end(), childHandles.begin(), childHandles.end());
        
        return vec;
        /*std::vector<Data> result;
        for (auto& elem : vec)
        {
            if (m_data.contains(elem))
                result.emplace_back(m_data.at(elem));
        }

        return result;*/
    }

    std::vector<SceneNode::handle_type> GetRootChilds(bool includeItself = false) const
    {
        return GetChilds(&m_root, includeItself);
    }

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
