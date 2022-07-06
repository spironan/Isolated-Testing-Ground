#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include "src/UUID.h"

#include "Scenegraph.h"

using Entity = size_t;
Scenegraph g_sceneGraph{"testSceneGraph"};    // global scenegraph
//std::unordered_map<oo::UUID, Entity> g_MasterMap;

class GameObject
{
private:
    static Entity index;
    Entity m_entt = index++;
    SceneNode m_node;

public:
    explicit GameObject(Entity entt)
        : m_entt{ entt }
        , m_node{ "explicit ctor", oo::UUID{} }
    {
    }

    GameObject(std::string const& name) : m_node{ name, oo::UUID{} }
    {
        //g_sceneGraph.AddChildToRoot(&m_node, m_entt);
        //g_MasterMap[m_node.getHandle()] = m_entt;
    }
    
    GameObject(std::string const& name, oo::UUID uuid) : m_node{ name, uuid }
    {
        //g_sceneGraph.AddChildToRoot(&m_node, m_entt);
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


    //GameObject GetParent()
    //{
    //    return static_cast<GameObject>(g_sceneGraph.GetParent(&m_node));
    //    //return static_cast<GameObject>(g_MasterMap.at(m_node.getParentHandle()));
    //}

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