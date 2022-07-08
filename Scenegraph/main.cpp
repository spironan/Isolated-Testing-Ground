#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include "src/UUID.h"

#include "scenegraph.h"

using Entity = size_t;
std::unique_ptr<scenegraph> g_sceneGraph;    // global scenegraph
//std::unordered_map<UUID, Entity> g_MasterMap;

class GameObject
{
private:
    static Entity index;
    Entity m_entt = index++;
    scenenode::shared_pointer m_node = nullptr;

public:
    explicit GameObject(Entity entt)
        : m_entt{ entt }
        , m_node{ g_sceneGraph->create_new_child("explicit ctor", UUID{}) }
    {
    }

    GameObject(std::string_view name) 
        : m_node{ g_sceneGraph->create_new_child(name, UUID{}) }
    {
        //g_sceneGraph->AddChildToRoot(&m_node, m_entt);
        //g_MasterMap[m_node.getHandle()] = m_entt;
    }
    
    GameObject(std::string_view name, UUID uuid) : m_node{ g_sceneGraph->create_new_child(name, uuid) }
    {
        //g_sceneGraph->AddChildToRoot(&m_node, m_entt);
    }

    void Print() const
    {
        std::cout << "Entity No : " << m_entt << "\n";
        g_sceneGraph->print();
    }

    Entity GetEntity() const
    {
        return m_entt;
    }

    void AddChild(GameObject& target)
    {
        scenegraph::add_child(m_node.get(), target.m_node.get());
    }


    //GameObject GetParent()
    //{
    //    return static_cast<GameObject>(g_sceneGraph->GetParent(&m_node));
    //    //return static_cast<GameObject>(g_MasterMap.at(m_node.getParentHandle()));
    //}

    std::vector<GameObject> GetChilds(bool includeItself = false)
    {
        std::vector<GameObject> res;
        for (auto& entt : g_sceneGraph->get_childs(m_node.get(), includeItself))
            res.emplace_back(entt);
        /*for (auto& uuid : g_sceneGraph->GetChilds(&m_node, includeItself))
            if (uuid != Node::NOTFOUND)
                res.emplace_back(g_sceneGraph->GetData(&m_node));*/
                //res.emplace_back(g_MasterMap.at(uuid));

        return res;
    }

    
    void Destroy()
    {
        m_node->detach();
    }

};
Entity GameObject::index = 0;

void PrintGo(GameObject const* go)
{
    go->Print();
}

int main() 
{
    // Memory Leak Checker in Debug builds
#if not defined (OO_PRODUCTION)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // Uncomment to cause a break on allocation for debugging
    //_CrtSetBreakAlloc(/*Allocation Number here*/);
#endif
    g_sceneGraph = std::make_unique<scenegraph>("Scenegraph");

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