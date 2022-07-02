#pragma once

#include "glm/glm.hpp"
#include "Quaternion.h"

#include <iostream>

struct Transform final
{
public:
    using vec3 = glm::vec3;
    using mat4 = glm::mat4;

    Transform();
    
    Transform(Transform const& other) = default;

    /*-----------------------------------------------------------------------------*/
    /* Getter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3        GetPosition()     const;
    quaternion  GetRotationQuat() const;
    vec3        GetEulerAngles()  const;
    vec3        GetScale()        const;

    vec3 LocalRight()          const;
    vec3 LocalUp()             const;
    vec3 LocalForward()        const;

    vec3 GlobalRight()         const;
    vec3 GlobalUp()            const;
    vec3 GlobalForward()       const;

    mat4 GetLocalMatrix()      const;
    mat4 GetGlobalMatrix()     const;

    bool HasChanged()          const;
    bool IsDirty()             const;

    /*-----------------------------------------------------------------------------*/
    /* Setter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3& Position();
    vec3& Scale();
    // note : scale must be set using setEulerAngle (internally uses quaternions)

    // Local Setters
    void SetPosition(vec3 const& pos);
    void SetEulerAngles(vec3 const& eulerAngle);
    void SetScale(vec3 const& scale);

    // Global Setters
    void SetGlobalPosition(vec3 const& position);
    void SetGlobalScale(vec3 const& scale);
    void SetGlobalAngle(vec3 const& euler_angles);
    void SetGlobalTransform(vec3 const& position, vec3 const& euler_angles, vec3 const& scale);
    //void SetGlobalTransform(glm::mat4 const& targetGlobalTransform);

    vec3 GetGlobalPosition() const;
    mat4 GetGlobalRotationMatrix() const;
    vec3 GetGlobalRotationRad() const;  // return vec3 for rotation of each component (euler angles)
    vec3 GetGlobalRotationDeg() const;  // return vec3 for rotation of each component (euler angles)
    vec3 GetGlobalScale() const;
    
    // Functions and variables in the section below should be wrapped and hidden
    // only allowing users to use the functions above this line.
public:
    void CalculateLocalTransform();
    void RecalculateLocalValues();

public:

    mat4                m_globalTransform;
    mat4                m_localTransform;

    vec3                m_position;
    vec3                m_eulerRotation;
    quaternion          m_orientation;      //used internally
    vec3                m_scale;

    bool                m_dirty;
    bool                m_hasChanged;
};

std::ostream& operator<< (std::ostream& os, Transform::vec3 const& vector);

std::ostream& operator<< (std::ostream& os, Transform::mat4 const& matrix);