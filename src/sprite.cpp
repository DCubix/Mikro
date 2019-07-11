#include "sprite.h"
#include "png.inl"

#include "log.h"

#include <fstream>
#include <iterator>
#include <cmath>

namespace mik {

	namespace util {
		u8 dither(i32 x, i32 y, u8 comp) {
			return u8(std::max(std::min(comp + DITHER_4x4[(x % 4) + (y % 4) * 4], 255.0), 0.0));
		}

		u8 palette(u8 r, u8 g, u8 b) {
#ifdef MIK_PRECISE_RGB
			u8 closest = 0;
			u8 minR = 255;
			u8 minG = 255;
			u8 minB = 255;
			for (u8 i = 0; i < MikPaletteSize; i++) {
				i32 pcol = i32(PALETTE[i]);
				i32 pr = (pcol & 0xFF0000) >> 16;
				i32 pg = (pcol & 0x00FF00) >> 8;
				i32 pb = (pcol & 0x0000FF);

				u8 dx = std::abs(r - pr);
				u8 dy = std::abs(g - pg);
				u8 dz = std::abs(b - pb);
				if ((dx + dy + dz) < (minR + minG + minB)) {
					minR = dx;
					minG = dy;
					minB = dz;
					closest = i;
				}
			}
			return closest;
#else
			r = r > 5 ? componentConvert(r) : r;
			g = g > 5 ? componentConvert(g) : g;
			b = b > 5 ? componentConvert(b) : b;
			return (r * 36 + g * 6 + b) % MikPaletteSize;
#endif
		}

		u8 componentConvert(u8 comp) {
			return u8(std::round((f32(comp) / 255.0f) * 5.0f));
		}

		u8 rgbConvert(u8 comp) {
			return u8(std::round((f32(comp) / 5.0f) * 255.0f));
		}
	}

	Sprite::Sprite(std::string const& fileName) {
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
				u32 j = i / 4;
				u32 x = j % width;
				u32 y = j / width;
				u8 r = util::componentConvert(util::dither(x, y, pngData[i + 0]));
				u8 g = util::componentConvert(util::dither(x, y, pngData[i + 1]));
				u8 b = util::componentConvert(util::dither(x, y, pngData[i + 2]));
				if (pngData[i + 3] < 127) {
					r = 5; g = 0; b = 5;
				}
				u8 index = util::palette(r, g, b);
				m_data.push_back(index);
			}
			m_width = u32(width);
			m_height = u32(height);
		} else {
			LogE("Failed to load \"", fileName, "\": Not found.");
		}
	}

	Sprite::Sprite(u32 width, u32 height) {
		m_width = width;
		m_height = height;
		m_data.reserve(m_width * m_height);
		for (u32 i = 0; i < m_width * m_height; i++) {
			m_data.push_back(0);
		}
	}

	void Sprite::dot(i32 x, i32 y, u8 r, u8 g, u8 b) {
		if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
		const u32 i = x + y * m_width;
		const u8 index = util::palette(r, g, b);
		m_data[i] = index;
	}

	void Sprite::clear(u8 r, u8 g, u8 b) {
		const u8 index = util::palette(r, g, b);
		std::fill(m_data.begin(), m_data.end(), index);
	}

	Color Sprite::get(i32 x, i32 y) {
		x = math::wrap(x, 0, i32(m_width));
		y = math::wrap(y, 0, i32(m_height));
		const u32 i = x + y * m_width;
		u32 pcol = PALETTE[m_data[i]];
		Color col;
		col.r = util::componentConvert((pcol & 0xFF0000) >> 16);
		col.g = util::componentConvert((pcol & 0x00FF00) >> 8);
		col.b = util::componentConvert((pcol & 0x0000FF));
		return col;
	}
}