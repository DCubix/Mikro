#include "audio.h"

#include "log.h"
#include <algorithm>

namespace mik {
	Sound::Sound(std::string const& fileName) {
		drwav wav;
		if (!drwav_init_file(&wav, fileName.c_str())) {
			LogE("Invalid WAV file: \"", fileName, "\"");
			return;
		}

		m_frequency = wav.sampleRate;
		m_data.resize(wav.totalPCMFrameCount * wav.channels);
		drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, m_data.data());

		drwav_uninit(&wav);
	}

	Sound::Sound(std::vector<u8> const& data) {
		drwav wav;
		if (!drwav_init_memory(&wav, data.data(), data.size())) {
			LogE("Invalid WAV file.");
			return;
		}

		m_frequency = wav.sampleRate;
		m_data.resize(wav.totalPCMFrameCount * wav.channels);
		drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, m_data.data());

		drwav_uninit(&wav);
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

	void MikAudio::playMusic(Sound* sound, f32 gain, f32 pan, f32 fade) {
		if (sound == nullptr) {
			LogE("Invalid music.");
			return;
		}
		m_fadeTime = 0.0f;
		m_fade = fade;
		if (m_currMusic.sound == nullptr) {
			m_currMusic.gain = gain;
			m_currMusic.pan = pan;
			m_currMusic.position = 0.0f;
			m_currMusic.sound = sound;
			m_musicState = MPlaying;
		} else {
			m_nextMusic.gain = gain;
			m_nextMusic.pan = pan;
			m_nextMusic.position = 0.0f;
			m_nextMusic.sound = sound;
			m_musicState = MFadingOut;
		}
	}

	void MikAudio::stopMusic(f32 fade) {
		if (m_currMusic.sound == nullptr) return;
		m_fadeTime = 0.0f;
		m_fade = fade;
		m_musicState = MFadingOut;
	}

	void MikAudio::mix(f32* out, u32 samples) {
		const f32 advance = 1.0f / m_frequency;
		f32 left = 0.0f, right = 0.0f;
		for (; samples > 0; samples--) {
			left = right = 0.0f;
			for (u32 i = 0; i < MikAudioVoices; i++) {
				Voice& voc = m_voices[i];
				if (voc.state == Voice::Playing) {
					if (!voc.ended()) {
						auto [l, r] = voc.sample(advance);
						left += l;
						right += r;
					} else resetVoice(i);
				}
			}

			f32 fade = m_fade <= 0.0f ? 1.0f : m_fadeTime / m_fade;

			if (m_currMusic.sound != nullptr) {
				// Mix music
				switch (m_musicState) {
					case MPlaying: {
						auto [l, r] = m_currMusic.sample(advance);
						left += l * fade;
						right += r * fade;
					} break;
					case MFadingOut: {
						if (fade < 1.0f) {
							auto [l, r] = m_currMusic.sample(advance);
							left += l * (1.0f - fade);
							right += r * (1.0f - fade);
						} else {
							m_musicState = MStopped;
							m_currMusic.sound = nullptr;
						}
					} break;
					default: break;
				}
			}

			// Fading/Switching
			if (m_nextMusic.sound != nullptr) {
				if (fade >= 1.0f) {
					m_currMusic.gain = m_nextMusic.gain;
					m_currMusic.pan = m_nextMusic.pan;
					m_currMusic.position = 0.0f;
					m_currMusic.sound = m_nextMusic.sound;
					m_nextMusic.sound = nullptr;
					m_musicState = MPlaying;
					m_fadeTime = 0.0f;
				}
			}

			if (m_fadeTime < m_fade)
				m_fadeTime += advance;

			left = std::clamp(left * m_gain, -1.0f, 1.0f);
			right = std::clamp(right * m_gain, -1.0f, 1.0f);
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

	Sample Voice::sample(f32 step) {
		f32 sample = std::clamp(sound->data()[u32(position)] * gain, -1.0f, 1.0f);
		f32 left = std::clamp(sample * (0.5f - pan), -1.0f, 1.0f);
		f32 right = std::clamp(sample * (0.5f + pan), -1.0f, 1.0f);
		position += f32(sound->frequency()) * step * pitch;
		return std::make_tuple(left, right);
	}

	Sample Music::sample(f32 step) {
		f32 sample = std::clamp(sound->data()[u32(position)] * gain, -1.0f, 1.0f);
		f32 left = std::clamp(sample * (0.5f - pan), -1.0f, 1.0f);
		f32 right = std::clamp(sample * (0.5f + pan), -1.0f, 1.0f);
		position += f32(sound->frequency()) * step;
		position = f32(u32(position) % sound->data().size());
		return std::make_tuple(left, right);
	}

}