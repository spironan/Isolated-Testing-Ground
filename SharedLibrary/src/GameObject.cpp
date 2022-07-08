#include "GameObject.h"

extern "C"
{
    // global scenegraph
    scenegraph g_sceneGraph{ "RootNode" };
    GameObject g_root{ g_sceneGraph.get_root() };
    std::unordered_map<scenegraph::handle_type, GameObject*> g_sceneData = { {UUID{0}, &g_root } };
}

Entity GameObject::index = 0;

GameObject::GameObject()
    : m_node{ g_sceneGraph.create_new_child("default", UUID{}) }
    , m_transform{ }
{
}

// for root creation only
GameObject::GameObject(scenenode::shared_pointer root_node)
    : m_node{ root_node }
    , m_transform{ }
{
}

GameObject::GameObject(Entity entt)
    : m_entt{ entt }
    , m_node{ g_sceneGraph.create_new_child("explicit ctor", UUID{}) }
    , m_transform { }
{
}

GameObject::GameObject(std::string const& name) 
    : m_node{ g_sceneGraph.create_new_child(name, UUID{}) }
    , m_transform{ }
{
    g_sceneData[m_node->get_handle()] = this;
}

GameObject::GameObject(std::string const& name, UUID uuid) 
    : m_node{ g_sceneGraph.create_new_child(name, uuid) }
    , m_transform{ }
{
    g_sceneData[m_node->get_handle()] = this;
}

void GameObject::Print() const
{
    std::cout << "Entity No : " << m_entt << "\n";
    m_node->print_recursive();
    //g_sceneGraph.Print();
}

Entity GameObject::GetEntity() const
{
    return m_entt;
}

void GameObject::AddChild(GameObject& target)
{
    scenegraph::add_child(m_node.get(), target.m_node.get());
}

GameObject GameObject::GetParent()
{
    return static_cast<GameObject>(*g_sceneData.at(m_node->get_parent_handle()));
    //return static_cast<GameObject>(g_MasterMap.at(m_node.getParentHandle()));
}

std::vector<GameObject> GameObject::GetChilds(bool includeItself)
{
    std::vector<GameObject> res;
    for (auto& entt : g_sceneGraph.get_childs(m_node.get(), includeItself))
        res.emplace_back(entt);
    /*for (auto& uuid : g_sceneGraph.GetChilds(&m_node, includeItself))
    if (uuid != Node::NOTFOUND)
    res.emplace_back(g_sceneGraph.GetData(&m_node));*/
    //res.emplace_back(g_MasterMap.at(uuid));

    return res;
}

void GameObject::Destroy()
{
    m_node->detach();
}

void PrintGo(GameObject const* go)
{
    go->Print();
}
