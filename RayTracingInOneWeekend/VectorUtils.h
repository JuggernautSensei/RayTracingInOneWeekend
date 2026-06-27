#pragma once

[[nodiscard]] Vec3 GenerateRandomUnitVecOnSphere();
[[nodiscard]] Vec3 GenerateRandomUnitVec3OnHemisphere(Vec3 _normal);
[[nodiscard]] Vec3 GenerateRandomVec3InDisk();
[[nodiscard]] Vec3 GammaCorrect(Vec3 _linearColor, float _gamma);
