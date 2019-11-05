#pragma once

#include "compat/math.hpp"

#include <Antilatency.Math.h>

namespace Antilatency {
    namespace Math {
        inline floatQ operator+(const floatQ& q1, const floatQ& q2) {
            return floatQ{
                q1.x + q2.x,
                q1.y + q2.y,
                q1.z + q2.z,
                q1.w + q2.w
            };
        }

        inline floatQ operator-(const floatQ& q1, const floatQ& q2) {
            return floatQ{
                q1.x - q2.x,
                q1.y - q2.y,
                q1.z - q2.z,
                q1.w - q2.w
            };
        }

        inline floatQ operator*(const floatQ& q1, const floatQ& q2) {
            return floatQ{
                (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
                (q1.w * q2.y) + (q1.y * q2.w) + (q1.z * q2.x) - (q1.x * q2.z),
                (q1.w * q2.z) + (q1.z * q2.w) + (q1.x * q2.y) - (q1.y * q2.x),
                (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z)
            };
        }

        inline float Size(const floatQ q) {
            return sqrtf((q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w));
        }

        inline float SizeSquared(const floatQ q) {
            return (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
        }

        inline floatQ Normalize(const floatQ q, const float t) {
            float squareSum = SizeSquared(q);

            if (squareSum >= t) {
                float scale = sqrtf(squareSum);
                return {
                    q.x * scale,
                    q.y * scale,
                    q.z * scale,
                    q.w * scale
                };
            } else {
                return { 0.0f, 0.0f, 0.0f, 1.0f };
            }
        }

        inline bool IsNormalized(const floatQ q) {
            return abs(1.0f - SizeSquared(q)) < 0.01f;
        }

        inline floatQ Inverse(const floatQ q) {
            return{-q.x, -q.y, -q.z, q.w};
        }

        inline bool IsNanAny(const floatQ q) {
            return (isnan(q.x) || isnan(q.y) || isnan(q.z) || isnan(q.w));
        }

        inline void ToEulerAngles(const floatQ q, double& yaw, double& pitch, double& roll) {
            /** assumes q1 is a normalised quaternion */
            const double rad2deg = 180.0 / M_PI;
            double test = q.x * q.y + q.z * q.w;
            if (test > 0.499) { // singularity at north pole
                yaw = 2.0 * atan2(q.x, q.w);
                roll = M_PI / 2.0;
                pitch = 0.0;
            }
            else if (test < -0.499) { // singularity at south pole
                yaw = -2.0 * atan2(q.x, q.w);
                roll = -M_PI / 2.0;
                pitch = 0.0;
                return;
            }
            else {
                double sqx = q.x * q.x;
                double sqy = q.y * q.y;
                double sqz = q.z * q.z;
                yaw = atan2(2.0 * q.y  * q.w - 2.0 * q.x * q.z, 1.0 - 2.0 * sqy - 2.0 * sqz);
                roll = asin(2.0 * test);
                pitch = atan2(2.0 * q.x * q.w - 2.0 * q.y * q.z, 1.0 - 2.0 * sqx - 2.0 * sqz);
            }

            yaw *= rad2deg;
            roll *= rad2deg;
            pitch *= rad2deg;
        }
    }
}

