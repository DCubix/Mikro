#ifndef MIK_BYTEUTILS_H
#define MIK_BYTEUTILS_H

#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include "types.h"

namespace mik {
	class ByteWriter {
	public:
		ByteWriter() = default;
		virtual ~ByteWriter() = default;

		template <typename T>
		u32 write(T value) {
			const size_t size = sizeof(T);
			std::vector<u8> bvalue;
			bvalue.resize(size);
			std::memcpy(bvalue.data(), &value, size);
			m_data.insert(m_data.end(), bvalue.begin(), bvalue.end());
			return size;
		}

		template <typename T>
		u32 writeAll(std::vector<T> const& value) {
			const size_t size = sizeof(T) * value.size();
			for (auto&& v : value) write(v);
			return size;
		}

		u8* data() { return m_data.data(); }
		std::vector<u8> dataVector() const { return m_data; }
		u32 size() const { return m_data.size(); }

	private:
		std::vector<u8> m_data{};
	};

	class ByteReader {
	public:
		ByteReader() = default;
		virtual ~ByteReader() = default;

		ByteReader(std::vector<u8> const& data) : m_data(data) {}

		template <typename T>
		T read() {
			const size_t size = sizeof(T);
			std::vector<u8> bvalue;
			bvalue.reserve(size);
			for (u32 i = 0; i < size; i++) {
				bvalue.push_back(m_data.front());
				m_data.erase(m_data.begin());
			}
			T value{};
			std::memcpy(&value, bvalue.data(), size);
			return value;
		}

		std::vector<u8> readAll(u32 count) {
			std::vector<u8> value{};
			value.reserve(count);
			for (u32 i = 0; i < count; i++) value.push_back(read<u8>());
			return value;
		}

		std::string readString(u32 count) {
			std::vector<u8> dat = readAll(count);
			return std::string(dat.begin(), dat.end());
		}

		u8 peek() { return m_data[0]; }
		u32 size() const { return m_data.size(); }

		void reverse() { std::reverse(m_data.begin(), m_data.end()); }

	private:
		std::vector<u8> m_data{};
	};
}

#endif // MIK_BYTEUTILS_H