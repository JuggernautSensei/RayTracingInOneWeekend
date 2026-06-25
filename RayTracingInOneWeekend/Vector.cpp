#include "pch.h"

#include "Vector.h"

Vec3 Vec3::kZero    = Vec3 { 0.f };
Vec3 Vec3::kOne     = Vec3 { 1.f };
Vec3 Vec3::kUnitX   = Vec3 { 1.f, 0.f, 0.f };
Vec3 Vec3::kUnitY   = Vec3 { 0.f, 1.f, 0.f };
Vec3 Vec3::kUnitZ   = Vec3 { 0.f, 0.f, 1.f };
Vec3 Vec3::kRight   = Vec3 { 1.f, 0.f, 0.f };
Vec3 Vec3::kUp      = Vec3 { 0.f, 1.f, 0.f };
Vec3 Vec3::kForward = Vec3 { 0.f, 0.f, 1.f };
