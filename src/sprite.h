#ifndef MIK_SPRITE_H
#define MIK_SPRITE_H

#include "types.h"
#include "animator.h"
#include <vector>
#include <string>

namespace mik {
	struct Color {
		u8 r{ 0 }, g{ 0 }, b{ 0 };
		bool ghost{ false };
	};

	class Sprite {
		public:
			Sprite() = default;
			virtual ~Sprite() = default;

			/// Load From PNG file
			Sprite(std::string const& fileName);

			/// Create blank
			Sprite(u32 width, u32 height);

			/// Draw a pixel
			void dot(i32 x, i32 y, u8 r, u8 g, u8 b, bool ghost = false);

			/// Clear
			void clear(u8 r = 0, u8 g = 0, u8 b = 0, bool ghost = false);

			/// Get pixel at X, Y
			Color get(i32 x, i32 y);

			Color* data() { return m_data.data(); }
			u32 width() const { return m_width; }
			u32 height() const { return m_height; }

			Animator& animator() { return m_animator; }

			u32 rows() const { return m_rows; }
			void rows(u32 v) { m_rows = v; }

			u32 cols() const { return m_cols; }
			void cols(u32 v) { m_cols = v; }

		private:
			Animator m_animator{};
			u32 m_rows{ 1 }, m_cols{ 1 };

			u32 m_width, m_height;
			std::vector<Color> m_data;
	};

	int decodePNG(
		std::vector<unsigned char>& out_image,
		unsigned long& image_width,
		unsigned long& image_height,
		const unsigned char* in_png,
		size_t in_size,
		bool convert_to_rgba32 = true
	);
}

#endif // MIK_SPRITE_H