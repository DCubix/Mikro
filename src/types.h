#ifndef MIK_TYPES_H
#define MIK_TYPES_H

#include <cstdint>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

namespace mik { namespace math {
	template <typename T>
	inline T wrap(T v, T vmin, T vmax) {
		return (((v - vmin) % (vmax - vmin)) + (vmax - vmin)) % (vmax - vmin) + vmin;
	}

	template <typename T>
	inline T clamp(T v, T vmin, T vmax) {
		return v < vmin ? vmin : v > vmax ? vmax : v;
	}

	inline f32 lerp(f32 a, f32 b, f32 t) {
		return (1.0f - t) * a + b * t;
	}
}}

#endif // MIK_TYPES_H