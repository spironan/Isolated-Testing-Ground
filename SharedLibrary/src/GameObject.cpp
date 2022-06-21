#include "GameObject.h"

extern "C"
{
    // global scenegraph
    Scenegraph g_sceneGraph{ "RootNode" };
    GameObject g_root{ g_sceneGraph.GetRoot() };
    std::unordered_map<Scenegraph::Key, GameObject*> g_sceneData = { {oo::UUID{0}, &g_root } };
}

Entity GameObject::index = 0;

GameObject::GameObject(Entity entt)
    : m_entt{ entt }
    , m_node{ "explicit ctor", oo::UUID{} }
    , m_transform { }
{
}

GameObject::GameObject(std::string const& name) 
    : m_node{ name, oo::UUID{} }
    , m_transform{ }
{
    g_sceneGraph.AddChildToRoot(&m_node);
    g_sceneData[m_node.getHandle()] = this;
}

GameObject::GameObject(std::string const& name, oo::UUID uuid) 
    : m_node{ name, uuid }
    , m_transform{ }
{
    g_sceneGraph.AddChildToRoot(&m_node);
    g_sceneData[m_node.getHandle()] = this;
}

void GameObject::Print() const
{
    std::cout << "Entity No : " << m_entt << "\n";
    m_node.printRecursive();
    //g_sceneGraph.Print();
}

Entity GameObject::GetEntity() const
{
    return m_entt;
}

void GameObject::AddChild(GameObject& target)
{
    g_sceneGraph.AddChild(&m_node, &target.m_node);
}

GameObject GameObject::GetParent()
{
    return static_cast<GameObject>(*g_sceneData.at(m_node.getParentHandle()));
    //return static_cast<GameObject>(g_MasterMap.at(m_node.getParentHandle()));
}

std::vector<GameObject> GameObject::GetChilds(bool includeItself)
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

void GameObject::Destroy()
{
    m_node.detach();
}

void PrintGo(GameObject const* go)
{
    go->Print();
}
