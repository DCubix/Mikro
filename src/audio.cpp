#include "audio.h"

#include "log.h"
#include <algorithm>

namespace mik {
	Sound::Sound(std::string const& fileName) {
		drwav* wav = drwav_open_file(fileName.c_str());
		if (wav == nullptr) {
			LogE("Invalid WAV file: \"", fileName, "\"");
			return;
		}

		m_frequency = wav->sampleRate;
		m_data.resize(wav->totalPCMFrameCount);
		drwav_read_f32(wav, wav->totalPCMFrameCount, m_data.data());
		drwav_close(wav);
	}

	MikAudio::MikAudio() {
		for (u32 i = 0; i < MikAudioVoices; i++) resetVoice(i);
		m_frequency = MikAudioFreq;
		m_gain = 1.0f;
	}

	Voice* MikAudio::play(Sound* sound, f32 gain, f32 pitch, f32 pan) {
		if (sound == nullptr) {
			LogE("Invalid sound.");
			return nullptr;
		}
		Voice* voice = getFreeVoice();
		voice->gain = std::max(gain, 0.0f);
		voice->pitch = std::clamp(pitch, 0.1f, 10.0f);
		voice->pan = std::clamp(pan, -1.0f, 1.0f);
		voice->sound = sound;
		voice->position = 0.0f;
		voice->state = Voice::Playing;
		return voice;
	}

	void MikAudio::stop(Sound* sound) {
		if (sound == nullptr) {
			LogE("Invalid sound.");
			return;
		}
		for (u32 i = 0; i < MikAudioVoices; i++) {
			if (m_voices[i].sound == sound) resetVoice(i);
		}
	}

	void MikAudio::mix(f32* out, u32 samples) {
		const f32 advance = 1.0f / m_frequency;
		f32 left = 0.0f, right = 0.0f;
		for (; samples > 0; samples--) {
			left = right = 0.0f;
			for (u32 i = 0; i < MikAudioVoices; i++) {
				Voice& voc = m_voices[i];
				if (voc.state == Voice::Playing) {
					u32 pos = u32(voc.position);
					if (pos < voc.sound->data().size()) {
						f32 sample = std::clamp(voc.sound->data()[pos] * voc.gain, -1.0f, 1.0f);
						left += std::clamp(sample * (0.5f - voc.pan), -1.0f, 1.0f);
						right += std::clamp(sample * (0.5f + voc.pan), -1.0f, 1.0f);
						voc.position += f32(voc.sound->frequency()) * advance * voc.pitch;
					} else resetVoice(i);
				}
			}
			left = std::clamp(left, -1.0f, 1.0f);
			right = std::clamp(right, -1.0f, 1.0f);
			*out++ = left;
			*out++ = right;
		}
	}

	void MikAudio::resetVoice(u32 id) {
		Voice& voice = m_voices[id];
		voice.sound = nullptr;
		voice.state = Voice::Stopped;
		voice.position = voice.gain = voice.pitch = voice.pan = 0.0f;
	}

	Voice* MikAudio::getFreeVoice() {
		Voice* voice = nullptr;
		for (auto&& voc : m_voices) {
			if (voc.state == Voice::Stopped) {
				voice = &voc;
				break;
			}
		}
		if (voice == nullptr) { // Steal
			voice = &m_voices[0];
		}
		return voice;
	}
}