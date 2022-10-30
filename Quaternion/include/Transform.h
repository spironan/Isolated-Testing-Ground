#pragma once

#ifndef GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#endif

#include <glm/glm.hpp>
#include "Quaternion.h"

#include <iostream>

struct Transform3D final
{
public:
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;
    using quat = quaternion;

    Transform3D() = default;
    Transform3D(Transform3D const& other) = default;

    /*-----------------------------------------------------------------------------*/
    /* Getter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3 GetEulerAnglesDeg()  const;
    vec3 GetEulerAnglesRad()  const;
    mat4 GetRotationMatrix()  const;

    vec3 Right()      const;
    vec3 Up()         const;
    vec3 Forward()    const;

    mat4 GetMatrix()  const;

    /*-----------------------------------------------------------------------------*/
    /* Setter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    void SetRotation(quat quaternion);
    void SetTransform(mat4 desired_transform);

    // Functions and variables in the section below should be wrapped and hidden
    // only allowing users to use the functions above this line.
    void CalculateTransform();
    void LookAt(vec3 target);
    void GlobalLookAtDir(vec3 direction);

    static void DecomposeValues(mat4 const matrix, glm::vec3& scale, glm::quat& orientation, glm::vec3& position);

    operator mat4() { return Matrix; }
    operator const mat4() const { return Matrix; }

public:
    mat4 Matrix = mat4{ 1.f };
    vec3 Position = vec3{ 0.f };
    quat Orientation = quat{ quat::identity() };          // used internally
    vec3 Scale = vec3{ 1.f };
};

static constexpr std::size_t transform_size = sizeof(Transform3D);

std::ostream& operator<< (std::ostream& os, Transform3D::vec3 const& vector);

std::ostream& operator<< (std::ostream& os, Transform3D::mat4 const& matrix);