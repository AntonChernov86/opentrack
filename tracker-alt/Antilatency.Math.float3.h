#pragma once

#include "compat/math.hpp"

#include <Antilatency.Math.h>

namespace Antilatency {
    namespace Math {
        inline float3 operator+(const float3& v1, const float3& v2) {
            return {
                v1.x + v2.x,
                v1.y + v2.y,
                v1.z + v2.z
            };
        }

        inline float3 operator-(const float3& v1, const float3& v2)  {
            return {
                v1.x - v2.x,
                v1.y - v2.y,
                v1.z - v2.z
            };
        }

        inline float3 operator*(const float3& v1, const float3& v2) {
            return {
                v1.x * v2.x,
                v1.y * v2.y,
                v1.z * v2.z
            };
        }

        inline float3 operator*(const float3& v, const float& f) {
            return{
                v.x * f,
                v.y * f,
                v.z * f
            };
        }

        inline float3 operator/(const float3& v1, const float3& v2)  {
            return {
                v1.x / v2.x,
                v1.y / v2.y,
                v1.z / v2.z
            };
        }

        inline float3 operator/(const float3& v, const float& f) {
            return{
                v.x / f,
                v.y / f,
                v.z / f
            };
        }

        inline bool IsNanAny(const float3& v) {
            return (isnan(v.x) || isnan(v.y) || isnan(v.z));
        }

        inline float3 Inverse(const float3& v) {
            return { -v.x, -v.y, -v.z };
        }           

        inline static float3 Right() {
            return { 1.0f, 0.0f, 0.0f };
        }

        inline static float3 Up() {
            return { 0.0f, 1.0f, 0.0f };
        }

        inline static float3 Forward() {
            return { 0.0f, 0.0f, 1.0f };
        }

        inline static float3 Zero() {
            return { 0.0f, 0.0f, 0.0f };
        }

        inline static float3 One() {
            return { 1.0f, 1.0f, 1.0f };
        }
    }
}


