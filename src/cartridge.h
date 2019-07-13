#ifndef MIK_CARTRIDGE_H
#define MIK_CARTRIDGE_H

#include "byteutils.h"
#include "types.h"

#include <string>
#include <vector>
#include <memory>

namespace mik {

	struct FileEntry {
		std::string type;
		u32 size;
		std::string name;
		std::vector<u8> data;
	};

	using FileEntryPtr = std::unique_ptr<FileEntry>;

	class Cartridge {
	public:
		Cartridge() = default;
		virtual ~Cartridge() = default;

		std::vector<u8> save();
		void load(std::vector<u8> const& data);

		std::vector<FileEntryPtr>& files() { return m_files; }

		const std::vector<u8>& script() const { return m_script; }
		std::vector<u8> scriptCopy() const { return m_script; }
		void script(std::vector<u8> const& s) { m_script = s; }

	private:
		std::vector<FileEntryPtr> m_files;
		std::vector<u8> m_script;
	};

	using CartridgePtr = std::unique_ptr<Cartridge>;

}

#endif // MIK_CARTRIDGE_H