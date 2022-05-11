#pragma once
#include <tuple>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/quaternion_float.hpp>

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T pi() { return static_cast<T>(3.14159253589793243846); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T rad_to_deg() { return static_cast<T>(180.0) / pi<T>(); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T deg_to_rad() { return pi<T>() / static_cast<T>(180.0); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T radians(T const& degrees) { return degrees * deg_to_rad<T>(); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
static constexpr T degrees(T const& radians) { return radians * rad_to_deg<T>(); }

struct quaternion
{
public:
    using euler             = glm::vec3;
    using matrix_type       = glm::mat3;

    using size_type         = std::size_t;
    using value_type        = glm::vec4::value_type;
    using reference         = value_type&;
    using const_reference   = value_type const&;

public:
    static quaternion identity();

    static quaternion conjugate(quaternion const& input);

    static quaternion inverse(quaternion const& input);

    static bool is_normalized(quaternion const& input);

    static glm::vec3 rotate(quaternion const& quat, glm::vec3 const& vec);

    static matrix_type to_matrix(quaternion const& quat);

    static quaternion from_matrix(matrix_type const& mat);

    static quaternion lerp(quaternion const& a, quaternion const& b, value_type weight);

    static quaternion normalize(quaternion const& input);

    static value_type angle_radians(quaternion const& lhs, quaternion const& rhs);

    static value_type angle_degrees(quaternion const& lhs, quaternion const& rhs);

    static value_type dot(quaternion const& lhs, quaternion const& rhs);
    
    static quaternion from_euler(euler const& angle_radians);

    static euler to_euler(quaternion const& q);

    static std::tuple<glm::vec3, value_type> to_axis_angle(quaternion q1) 
    {
        value_type angle;
        glm::vec3 axis{};
        if (q1.w > 1.0f) q1.normalize();                    // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
        angle = 2 * std::acos(q1.w);
        value_type s = std::sqrt(1.0f - q1.w * q1.w);       // assuming quaternion normalised then w is less than 1, so term always positive.
        if (s < std::numeric_limits<value_type>::epsilon()) // test to avoid divide by zero, s is always positive due to sqrt
        { 
            // if s close to zero then direction of axis not important
            axis.x = q1.x;   // if it is important that axis is normalised then replace with x=1; y=z=0;
            axis.y = q1.y;
            axis.z = q1.z;
        }
        else 
        {
            axis.x = q1.x / s; // normalise axis
            axis.y = q1.y / s;
            axis.z = q1.z / s;
        }

        return { axis, angle };
    }

    static quaternion from_axis_angle(glm::vec3 axis_normalized, value_type angle)
    {
        return { axis_normalized, angle };
    }

public:

    // i j k w
    // x y z w
    // Quarternion given an axis angle : make sure direction is normalized.
    quaternion(glm::vec3 dir_normalized, value_type angle_radians);
    
    quaternion(value_type x, value_type y, value_type z, value_type w);

    friend quaternion operator*(quaternion const& lhs, quaternion const& rhs);
    /*
    {
        auto Pv = glm::vec3{ lhs.x, lhs.y, lhs.z };
        auto Qv = glm::vec3{ rhs.x, rhs.y, rhs.z };
        auto vecRes = lhs.w * (Qv) + rhs.w * (Pv) + glm::cross(Pv, Qv);
        auto scalarRes = lhs.w * rhs.w - glm::dot(Pv, Qv);
        return {vecRes, scalarRes};
    }*/
    
    quaternion normalize();

    const_reference operator[](size_type index) const;

    reference operator[](size_type index);

    value_type x, y, z, w;

    
};

