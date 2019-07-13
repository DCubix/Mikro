#include "cartridge.h"

#include <memory>
#include <fstream>
#include <iterator>

#include "log.h"

/**
 *    Cartridge Format
 * +----------------------------------+
 * | IDENTIFIER (3 bytes) "MIK"       |
 * +----------------------------------+
 * | FILE COUNT (2 bytes)             |
 * +----------------------------------+
 * | FILE ENTRIES                     |
 * |     TYPE: 3 bytes (WAV,PNG)      |
 * |     SIZE: 4 bytes                |
 * |     NAME: Null-ternimated string |
 * | e.g: WAV3245punch.wav ...        |
 * +----------------------------------+
 * | SCRIPT SIZE (4 bytes)            |
 * +----------------------------------+
 * | SCRIPT SECTION                   |
 * +----------------------------------+
 * | DATA SECTION                     |
 * +----------------------------------+
 */


namespace mik {

	std::vector<u8> Cartridge::save() {
		std::unique_ptr<ByteWriter> bw = std::make_unique<ByteWriter>();
		bw->write('M');
		bw->write('I');
		bw->write('K');

		bw->write(u16(m_files.size()));
		for (auto&& fe : m_files) {
			bw->write(fe->type[0]);
			bw->write(fe->type[1]);
			bw->write(fe->type[2]);
			bw->write(fe->size);
			for (u32 i = 0; i < fe->name.size(); i++) {
				bw->write(fe->name.at(i));
			}
			bw->write('\0');
		}
		bw->write(u32(m_script.size()));
		bw->writeAll(m_script);

		for (auto&& fe : m_files) {
			bw->writeAll(fe->data);
		}

		return bw->dataVector();
	}

	void Cartridge::load(std::vector<u8> const& data) {
		m_files.clear();
		m_script.clear();

		std::unique_ptr<ByteReader> br = std::make_unique<ByteReader>(data);
		if (br->readString(3) != "MIK") {
			LogE("Invalid cartridge file.");
			return;
		}

		u16 fileCount = br->read<u16>();
		for (u32 i = 0; i < fileCount; i++) {
			FileEntry* fe = new FileEntry();
			fe->type = br->readString(3);
			fe->size = br->read<u32>();
			fe->name = "";
			while (br->peek() != 0) {
				fe->name += br->read<u8>();
			}
			br->read<u8>();
			m_files.push_back(FileEntryPtr(fe));
		}

		u32 scriptSize = br->read<u32>();
		m_script = br->readAll(scriptSize);

		for (auto&& fe : m_files) {
			fe->data = br->readAll(fe->size);
		}
	}

}