#include "bitmap.h"
#include "png.inl"

#include "log.h"

#include <fstream>
#include <iterator>

namespace mik {
	Bitmap::Bitmap(std::string const& fileName) {
		std::ifstream fp(fileName, std::ios::binary);
		if (fp.good()) {
			fp.unsetf(std::ios::skipws);

			fp.seekg(0, std::ios::end);
			auto sz = fp.tellg();
			fp.seekg(0, std::ios::beg);

			std::vector<u8> data;
			data.reserve(sz);
			data.insert(data.begin(), std::istream_iterator<u8>(fp), std::istream_iterator<u8>());

			fp.close();

			std::vector<unsigned char> pngData;
			unsigned long width, height;
			if (decodePNG(pngData, width, height, data.data(), data.size()) != 0) {
				LogE("Failed to load \"", fileName, "\": Invalid PNG image.");
				return;
			}

			m_data.reserve(width * height);
			for (u32 i = 0; i < pngData.size(); i += 4) {
				m_data.push_back({ .r = pngData[i + 0], .g = pngData[i + 1], .b = pngData[i + 2], .ghost = pngData[i + 3] < 127 });
			}
			m_width = u32(width);
			m_height = u32(height);
		} else {
			LogE("Failed to load \"", fileName, "\": Not found.");
		}
	}

	Bitmap::Bitmap(u32 width, u32 height) {
		m_width = width;
		m_height = height;
		m_data.reserve(m_width * m_height);
		for (u32 i = 0; i < m_width * m_height; i++) {
			m_data.push_back({ .r = 0, .g = 0, .b = 0, .ghost = true });
		}
	}

	void Bitmap::dot(i32 x, i32 y, u8 r, u8 g, u8 b, bool ghost) {
		if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
		const u32 i = x + y * m_width;
		m_data[i].r = r;
		m_data[i].g = g;
		m_data[i].b = b;
		m_data[i].ghost = ghost;
	}

	void Bitmap::clear(u8 r, u8 g, u8 b, bool ghost) {
		for (u32 i = 0; i < m_width * m_height; i++) {
			m_data[i].r = r;
			m_data[i].g = g;
			m_data[i].b = b;
			m_data[i].ghost = ghost;
		}
	}

	Color Bitmap::get(i32 x, i32 y) {
		x = math::wrap(x, 0, i32(m_width));
		y = math::wrap(y, 0, i32(m_height));
		const u32 i = x + y * m_width;
		return m_data[i];
	}
}