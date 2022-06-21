#include <iostream>
#include <memory>
#include <vector>

#include "UUID.h"
#include <unordered_map>
#include <cassert>

class Node
{
public:
    using handle_type = size_t;
    static constexpr handle_type NOTFOUND = static_cast<handle_type>(-1);
private:
    // contains handle to actual data in scene-graph hierarchy
    Node* m_parent = nullptr;
    std::vector<Node*> m_childs = {};
    std::string m_debugName = "default name";
    handle_type m_handle = NOTFOUND;

    void printRecursive(size_t depth, bool printParent = true, bool printChilds = true) const
    {
        std::cout << "(" << depth << ") - "<< m_debugName << " [" << m_handle << "]";
        
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
            for(size_t i = 0; i < depth; ++i)
                std::cout << "\t";

            child->printRecursive(depth + 1);
        }
    }
    
    void remove(Node* node)
    {
        m_childs.erase(std::remove(m_childs.begin(), m_childs.end(), node), m_childs.end());
    }

public:
    Node(std::string const& name, handle_type handle) : m_debugName{ name }, m_handle{ handle } {}

    auto operator<=>(Node const&) const = default;

    bool contains(Node* node) const
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

    void detach()
    {
        if (m_parent != nullptr)
        {
            m_parent->remove(this);
            m_parent = nullptr;
        }
    }

    void addChild(Node* node) 
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

    size_t getDirectChildCount() const
    {
        return m_childs.size();
    }

    size_t getTotalChildCount() const
    {
        size_t count = m_childs.size();
        
        for (auto& child : m_childs)
            count += child->getTotalChildCount();
        
        return count;
    }

    handle_type getHandle() const 
    {
        return m_handle;
    }
    
    handle_type getParentHandle() const
    {
        return m_parent ? m_parent->m_handle : NOTFOUND;
    }
    
    std::vector<handle_type> getAllChildHandles() const
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

    void printRecursive() const
    {
        printRecursive(0);
    }
};

void Print(Node const* node)
{
    // "Computation" with pointer-to-const
    const Node* root_ptr = node;
    root_ptr->printRecursive();
    size_t directChilds = root_ptr->getDirectChildCount();
    size_t totalChilds = root_ptr->getTotalChildCount();
    
    std::cout << "direct childs : " << directChilds << std::endl;
    std::cout << "total childs : " << totalChilds << std::endl;
}

// System will have one scenegraph object at any point in time.
template<typename Data>
struct Scenegraph
{
public:
    using Key = oo::UUID;
private:
    
    Node m_root;
    std::unordered_map<Key, Data> m_data;
public:
    Scenegraph(std::string const& name) : m_root{ name, 0 } {}

    void Print() const
    {
        m_root.printRecursive();
    }

    void AddChildToRoot(Node* child, Data data)
    {
        m_root.addChild(child);
        m_data[child->getHandle()] = data;
    }

    void AddChild(Node* parent, Node* child)
    {
        parent->addChild(child);
    }

    Node GetRoot() const
    {
        return m_root;
    }

    std::vector<Data> GetChilds(Node const* target, bool includeItself) const
    {
        auto vec = target->getAllChildHandles();
        std::vector<Data> result;
        for (auto& elem : vec)
        {
            if (m_data.contains(elem))
                result.emplace_back(m_data.at(elem));
        }

        if (includeItself)
        {

        }

        return result;
    }

    Data GetParent(Node const * target) const
    {
        return GetData(target->getParentHandle());
    }

    Data GetData(Node const* target) const
    {
        assert(target == nullptr);
        assert(m_data.contains(target->getHandle()) == false);

        return m_data.at(target->getHandle());
    }

    Data GetData(Key handle) const
    {
        return m_data.at(handle);
    }
};


using Entity = size_t;
Scenegraph<Entity> g_sceneGraph{"testSceneGraph"};    // global scenegraph
//std::unordered_map<oo::UUID, Entity> g_MasterMap;

class GameObject
{
private:
    static Entity index;
    Entity m_entt = index++;
    Node m_node;

public:
    explicit GameObject(Entity entt)
        : m_entt{ entt }
        , m_node{ "explicit ctor", oo::UUID{} }
    {
    }

    GameObject(std::string const& name) : m_node{ name, oo::UUID{} }
    {
        g_sceneGraph.AddChildToRoot(&m_node, m_entt);
        //g_MasterMap[m_node.getHandle()] = m_entt;
    }
    
    GameObject(std::string const& name, oo::UUID uuid) : m_node{ name, uuid }
    {
        g_sceneGraph.AddChildToRoot(&m_node, m_entt);
    }

    void Print() const
    {
        std::cout << "Entity No : " << m_entt << "\n";
        g_sceneGraph.Print();
    }

    Entity GetEntity() const
    {
        return m_entt;
    }

    void AddChild(GameObject& target)
    {
        g_sceneGraph.AddChild(&m_node, &target.m_node);
    }


    GameObject GetParent()
    {
        return static_cast<GameObject>(g_sceneGraph.GetParent(&m_node));
        //return static_cast<GameObject>(g_MasterMap.at(m_node.getParentHandle()));
    }

    std::vector<GameObject> GetChilds(bool includeItself = false)
    {
        std::vector<GameObject> res;
        for (auto& entt : g_sceneGraph.GetChilds(&m_node, includeItself))
            res.emplace_back(entt);
        /*for (auto& uuid : g_sceneGraph.GetChilds(&m_node, includeItself))
            if (uuid != Node::NOTFOUND)
                res.emplace_back(g_sceneGraph.GetData(&m_node));*/
                //res.emplace_back(g_MasterMap.at(uuid));

        return res;
    }

    
    void Destroy()
    {
        m_node.detach();
    }

};
Entity GameObject::index = 0;

void PrintGo(GameObject const* go)
{
    go->Print();
}

int main() 
{
    GameObject go("go");
    GameObject go1("go1");
    GameObject go2("go2");
    go.AddChild(go1);
    go1.AddChild(go2);
    //go2.AddChild(go);
    //go.Destroy();
    //go1.Destroy();
    //go2.Destroy();
    PrintGo(&go);

    std::cout << "getting child of " << go.GetEntity() << std::endl;
    for (auto& child : go.GetChilds())
        std::cout << "\t entity " << child.GetEntity() << "\n";

}