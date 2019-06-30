#ifndef MIK_BITMAP_H
#define MIK_BITMAP_H

#include "types.h"
#include <vector>
#include <string>

namespace mik {
	struct Color {
		u8 r{ 0 }, g{ 0 }, b{ 0 };
		bool ghost{ false };
	};

	class Bitmap {
		public:
			Bitmap() = default;
			virtual ~Bitmap() = default;

			/// Load From PNG file
			Bitmap(std::string const& fileName);

			/// Create blank
			Bitmap(u32 width, u32 height);

			/// Draw a pixel
			void dot(i32 x, i32 y, u8 r, u8 g, u8 b, bool ghost = false);

			/// Clear
			void clear(u8 r = 0, u8 g = 0, u8 b = 0, bool ghost = false);

			/// Get pixel at X, Y
			Color get(i32 x, i32 y);

			Color* data() { return m_data.data(); }
			u32 width() const { return m_width; }
			u32 height() const { return m_height; }

		private:
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

#endif // MIK_BITMAP_H