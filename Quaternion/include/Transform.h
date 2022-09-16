#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Quaternion.h"

#include <iostream>

struct Transform final
{
public:
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;
    using quat = quaternion;

    Transform() = default;

    Transform(Transform const& other) = default;

    /*-----------------------------------------------------------------------------*/
    /* Getter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3 GetPosition()     const;
    quat GetRotationQuat() const;
    vec3 GetEulerAngles()  const;
    vec3 GetScale()        const;

    vec3 LocalRight()      const;
    vec3 LocalUp()         const;
    vec3 LocalForward()    const;

    vec3 GlobalRight()     const;
    vec3 GlobalUp()        const;
    vec3 GlobalForward()   const;

    mat4 GetLocalMatrix()  const;
    mat4 GetGlobalMatrix() const;

    vec3 GetGlobalPosition() const;
    mat4 GetGlobalRotationMatrix() const;
    vec3 GetGlobalRotationRad() const;  // return vec3 for rotation of each component (euler angles)
    vec3 GetGlobalRotationDeg() const;  // return vec3 for rotation of each component (euler angles)
    quat GetGlobalRotationQuat() const;
    vec3 GetGlobalScale() const;

    /*-----------------------------------------------------------------------------*/
    /* Setter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3& Position();
    vec3& Scale();
    // note : scale must be set using setEulerAngle (internally uses quaternions)

    // Local Setters
    void SetPosition(vec3 pos);
    void SetRotation(quat quaternion);
    void SetScale(vec3 scale);
    void SetLocalTransform(mat4 desired_local_transform);

    // Global Setters
    void SetGlobalPosition(vec3 position);
    void SetGlobalRotation(quat quaternion);
    void SetGlobalScale(vec3 scale);
    void SetGlobalTransform(vec3 position, vec3 euler_angles_degrees, vec3 scale);
    void SetGlobalTransform(mat4 desired_global_transform);


    // Functions and variables in the section below should be wrapped and hidden
    // only allowing users to use the functions above this line.
    void CalculateLocalTransform();
    void CalculateGlobalTransform();
    void DecomposeValues(mat4 matrix, glm::vec3& scale, glm::quat& orientation, glm::vec3& position);

    void LookAt(vec3 target);
public:

    mat4 m_globalTransform = mat4{ 1.f };
    mat4 m_localTransform = mat4{ 1.f };

    vec3 m_globalPosition = vec3{ 0.f };
    quat m_globalOrientation = quat{ quat::identity() };    // used internally
    vec3 m_globalScale = vec3{ 1.f };

    vec3 m_position = vec3{ 0.f };
    quat m_orientation = quat{ quat::identity() };          // used internally
    vec3 m_scale = vec3{ 1.f };

};

static constexpr std::size_t transform_size = sizeof(Transform);

std::ostream& operator<< (std::ostream& os, Transform::vec3 const& vector);

std::ostream& operator<< (std::ostream& os, Transform::mat4 const& matrix);