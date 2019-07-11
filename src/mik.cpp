#include "mik.h"

#include "log.h"
//#include "frame.h"

#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace mik {

	static void mik_audio_callback(void* userdata, Uint8* stream, int len) {
		MikAudio* audio = static_cast<MikAudio*>(userdata);
		audio->mix((float*) stream, len / (sizeof(float) * 2));
	}

	static std::map<u32, MikEvent> EventMapping = {
		{ SDLK_s, MikButtonLeft }, { SDLK_LEFT, MikButtonLeft },
		{ SDLK_f, MikButtonRight }, { SDLK_RIGHT, MikButtonRight },
		{ SDLK_e, MikButtonUp }, { SDLK_UP, MikButtonUp },
		{ SDLK_d, MikButtonDown }, { SDLK_DOWN, MikButtonDown },
		{ SDLK_z, MikButtonA }, { SDLK_k, MikButtonA },
		{ SDLK_x, MikButtonB }, { SDLK_l, MikButtonB },
		{ SDLK_KP_ENTER, MikButtonStart }, { SDLK_RETURN, MikButtonStart },

		{ SDL_BUTTON_LEFT, MikMouseLeft },
		{ SDL_BUTTON_RIGHT, MikMouseRight },
		{ SDL_BUTTON_MIDDLE, MikMouseMiddle }
	};

	static std::map<std::string, MikEvent> EventNames = {
		{ "left", MikButtonLeft },
		{ "right", MikButtonRight },
		{ "up", MikButtonUp },
		{ "down", MikButtonDown },
		{ "a", MikButtonA },
		{ "b", MikButtonB },
		{ "start", MikButtonStart },
		{ "left_mouse", MikMouseLeft },
		{ "middle_mouse", MikMouseMiddle },
		{ "right_mouse", MikMouseRight },

		{ "l", MikButtonLeft },
		{ "r", MikButtonRight },
		{ "u", MikButtonUp },
		{ "d", MikButtonDown },

		{ "ml", MikMouseLeft },
		{ "mm", MikMouseMiddle },
		{ "mr", MikMouseRight }
	};

	Mik::Mik(MikGame* game)
		:	m_game(std::unique_ptr<MikGame>(game)),
			m_audio(std::make_unique<MikAudio>())
	{}

	void Mik::run() {
		if (!m_game) {
			LogE("No game specified.");
			return;
		}

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			LogE(SDL_GetError());
			return;
		}

		m_window = SDL_CreateWindow(
			"Mik",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			MikScreenWidth * MikScreenUpscale, MikScreenHeight * MikScreenUpscale,
			SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
		);
		if (m_window == nullptr) {
			SDL_Quit();
			LogE(SDL_GetError());
			return;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
		if (m_renderer == nullptr) {
			SDL_Quit();
			LogE(SDL_GetError());
			return;
		}

		m_buf = SDL_CreateTexture(
			m_renderer,
			SDL_PIXELFORMAT_RGB24,
			SDL_TEXTUREACCESS_STREAMING,
			MikScreenWidth, MikScreenHeight
		);

		// Frame
		// std::vector<unsigned char> imgData;
		// unsigned long w, h;
		// decodePNG(imgData, w, h, frame_png, frame_png_len);
		// m_frame = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
		// SDL_UpdateTexture(m_frame, nullptr, imgData.data(), w * 4);
		//

		m_buffer = Sprite(MikScreenWidth, MikScreenHeight);
		m_clip[0] = m_clip[1] = m_clip[2] = m_clip[3] = 0;
		m_key = { 5, 0, 5 };

		f64 accum = 0.0;
		f64 lastTime = f64(SDL_GetTicks()) / 1000.0;

		SDL_AudioSpec want, have;
		want.format = AUDIO_F32;
		want.freq = MikAudioFreq;
		want.channels = 2;
		want.userdata = m_audio.get();
		want.samples = 4096;
		want.callback = mik_audio_callback;

		m_device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
		SDL_PauseAudioDevice(m_device, 0);

		LogI("Mik has been started successfully!");

		SDL_LockAudioDevice(m_device);
		m_game->onInit(*this);
		SDL_UnlockAudioDevice(m_device);

		SDL_Event evt;
		m_running = true;
		while (m_running) {
			bool canRender = false;
			f64 currTime = f64(SDL_GetTicks()) / 1000.0;
			f64 delta = currTime - lastTime;
			lastTime = currTime;
			accum += delta;

			while (SDL_PollEvent(&evt)) {
				switch (evt.type) {
					case SDL_QUIT: quit(); break;
					case SDL_KEYDOWN: {
						u32 k = evt.key.keysym.sym;
						if (k == SDLK_ESCAPE) quit();

						if (EventMapping.find(k) != EventMapping.end()) {
							m_input[EventMapping[k]].pressed = true;
							m_input[EventMapping[k]].held = true;
						}
					} break;
					case SDL_KEYUP: {
						u32 k = evt.key.keysym.sym;
						if (EventMapping.find(k) != EventMapping.end()) {
							m_input[EventMapping[k]].released = true;
							m_input[EventMapping[k]].held = false;
						}
					} break;
					case SDL_MOUSEBUTTONDOWN: {
						u32 b = evt.button.button;
						if (EventMapping.find(b) != EventMapping.end()) {
							m_input[EventMapping[b]].pressed = true;
							m_input[EventMapping[b]].held = true;
						}
						m_mouseX = evt.button.x / MikScreenUpscale;
						m_mouseY = evt.button.y / MikScreenUpscale;
					} break;
					case SDL_MOUSEBUTTONUP: {
						u32 b = evt.button.button;
						if (EventMapping.find(b) != EventMapping.end()) {
							m_input[EventMapping[b]].released = true;
							m_input[EventMapping[b]].held = false;
						}
						m_mouseX = evt.button.x / MikScreenUpscale;
						m_mouseY = evt.button.y / MikScreenUpscale;
					} break;
					case SDL_MOUSEMOTION: {
						m_mouseX = evt.motion.x / MikScreenUpscale;
						m_mouseY = evt.motion.y / MikScreenUpscale;
					} break;
				}
			}

			while (accum >= MikTimeStep) {
				/// Update game
				SDL_LockAudioDevice(m_device);
				m_game->onUpdate(*this, f32(MikTimeStep));
				SDL_UnlockAudioDevice(m_device);

				/// Update animators
				for (auto& spr : m_sprites) {
					spr->animator().update(f32(MikTimeStep));
				}

				accum -= MikTimeStep;
				canRender = true;

				for (size_t i = 0; i < MikEventCount; i++) {
					m_input[i].pressed = false;
					m_input[i].released = false;
				}
			}

			if (canRender) {
				m_game->onDraw(*this);

				// SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
				// SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
				SDL_RenderClear(m_renderer);

				u8* pixels;
				int pitch;
				SDL_LockTexture(m_buf, nullptr, (void**)&pixels, &pitch);
				for (u32 y = 0; y < MikScreenHeight; y++) {
					for (u32 x = 0; x < MikScreenWidth; x++) {
						Color col = m_buffer.get(x, y);
						col.r = util::rgbConvert(col.r);
						col.g = util::rgbConvert(col.g);
						col.b = util::rgbConvert(col.b);
						const u32 di = (x + y * MikScreenWidth) * 3;
						pixels[di + 0] = col.r;
						pixels[di + 1] = col.g;
						pixels[di + 2] = col.b;
					}
				}
				SDL_UnlockTexture(m_buf);

				// SDL_RenderCopy(m_renderer, m_frame, nullptr, nullptr);

				SDL_Rect dst = { 0, 0, MikScreenWidth, MikScreenHeight };
				SDL_RenderCopy(m_renderer, m_buf, nullptr, &dst);
				SDL_RenderPresent(m_renderer);
			}
		}
		m_game->onExit(*this);

		SDL_PauseAudioDevice(m_device, 1);
		SDL_CloseAudioDevice(m_device);

		m_sprites.clear();

		LogI("Mik has been stopped successfully!");

		SDL_DestroyTexture(m_buf);
		// SDL_DestroyTexture(m_frame);
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	/// Mik impl [INPUT]
	bool Mik::buttonPressed(std::string const& name) {
		std::string nam = name;
		std::transform(nam.begin(), nam.end(), nam.begin(), ::tolower);
		if (EventNames.find(nam) == EventNames.end()) {
			LogE("\"", name, "\" is not a valid button name.");
			return false;
		}

		const MikEvent evt = EventNames[nam];
		return m_input[evt].pressed;
	}

	bool Mik::buttonReleased(std::string const& name) {
		std::string nam = name;
		std::transform(nam.begin(), nam.end(), nam.begin(), ::tolower);
		if (EventNames.find(nam) == EventNames.end()) {
			LogE("\"", name, "\" is not a valid button name.");
			return false;
		}

		const MikEvent evt = EventNames[nam];
		return m_input[evt].released;
	}

	bool Mik::buttonHeld(std::string const& name) {
		std::string nam = name;
		std::transform(nam.begin(), nam.end(), nam.begin(), ::tolower);
		if (EventNames.find(nam) == EventNames.end()) {
			LogE("\"", name, "\" is not a valid button name.");
			return false;
		}

		const MikEvent evt = EventNames[nam];
		return m_input[evt].held;
	}

	void Mik::warpMouse(i32 x, i32 y) {
		SDL_WarpMouseInWindow(m_window, x * MikScreenUpscale, y * MikScreenUpscale);
	}

	std::string Mik::title() const {
		auto title = SDL_GetWindowTitle(m_window);
		return std::string(title);
	}

	void Mik::setTitle(std::string const& title) {
		SDL_SetWindowTitle(m_window, title.c_str());
	}

	/// Mik impl [GFX]
	bool Mik::clipEnabled() const {
		return m_clip[0] * m_clip[1] + m_clip[2] * m_clip[3] != 0;
	}

	bool Mik::pointInClip(i32 x, i32 y) const {
		return x >= m_clip[0] &&
				x < m_clip[2] &&
				y >= m_clip[1] &&
				y < m_clip[3];
	}

	void Mik::clip(u32 x, u32 y, u32 w, u32 h) {
		if (x * y + (x+w) * (y+h) == 0) {
			m_clip[0] = 0;
			m_clip[1] = 0;
			m_clip[2] = MikScreenWidth;
			m_clip[3] = MikScreenHeight;
			return;
		}
		m_clip[0] = x;
		m_clip[1] = y;
		m_clip[2] = x + w;
		m_clip[3] = y + h;
	}

	void Mik::unclip() {
		clip(0, 0, 0, 0);
	}

	void Mik::dot(i32 x, i32 y, u8 r, u8 g, u8 b) {
		if (!pointInClip(x, y) && clipEnabled()) return;
		if (m_key.r == r && m_key.g == g && m_key.b == b) return;
		m_buffer.dot(x, y, r, g, b);
	}

	void Mik::clear(u8 r, u8 g, u8 b) {
		rectf(0, 0, MikScreenWidth, MikScreenHeight, r, g, b);
	}

	void Mik::line(i32 x1, i32 y1, i32 x2, i32 y2, u8 r, u8 g, u8 b) {
		i32 dx = std::abs(x2 - x1);
		i32 sx = x1 < x2 ? 1 : -1;
		i32 dy = -std::abs(y2 - y1);
		i32 sy = y1 < y2 ? 1 : -1;
		i32 err = dx + dy;
		i32 e2 = 0;

		i32 x = x1;
		i32 y = y1;

		while (true) {
			dot(x, y, r, g, b);
			if (x == x2 && y == y2) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x += sx; }
			if (e2 <= dx) { err += dx; y += sy; }
		}
	}

	void Mik::rect(i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b) {
		line(x, y, x + w, y, r, g, b);
		line(x + w, y, x + w, y + h, r, g, b);
		line(x + w, y + h, x, y + h, r, g, b);
		line(x, y + h, x, y, r, g, b);
	}

	void Mik::rectf(i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b) {
		for (i32 ry = 0; ry < h; ry++) {
			line(x, ry + y, x + w, ry + y, r, g, b);
		}
	}

	void Mik::circ(i32 x1, i32 y1, i32 radius, u8 r, u8 g, u8 b) {
		i32 x = radius-1;
		i32 y = 0;
		i32 dx = 1;
		i32 dy = 1;
		i32 err = dx - (radius << 1);

		while (x >= y) {
			dot(x1 + x, y1 + y, r, g, b);
			dot(x1 + y, y1 + x, r, g, b);
			dot(x1 - y, y1 + x, r, g, b);
			dot(x1 - x, y1 + y, r, g, b);
			dot(x1 - x, y1 - y, r, g, b);
			dot(x1 - y, y1 - x, r, g, b);
			dot(x1 + y, y1 - x, r, g, b);
			dot(x1 + x, y1 - y, r, g, b);

			if (err <= 0) {
				y++;
				err += dy;
				dy += 2;
			}

			if (err > 0) {
				x--;
				dx += 2;
				err += dx - (radius << 1);
			}
		}
	}

	void Mik::circf(i32 x1, i32 y1, i32 radius, u8 r, u8 g, u8 b) {
		i32 x = radius-1;
		i32 y = 0;
		i32 dx = 1;
		i32 dy = 1;
		i32 err = dx - (radius << 1);

		while (x >= y) {
			line(x1 - y, y1 + x, x1 + y, y1 + x, r, g, b);
			line(x1 - x, y1 + y, x1 + x, y1 + y, r, g, b);
			line(x1 - x, y1 - y, x1 + x, y1 - y, r, g, b);
			line(x1 - y, y1 - x, x1 + y, y1 - x, r, g, b);

			if (err <= 0) {
				y++;
				err += dy;
				dy += 2;
			}

			if (err > 0) {
				x--;
				dx += 2;
				err += dx - (radius << 1);
			}
		}
	}

	void Mik::spr(Sprite* spr, i32 x, i32 y, i32 sx, i32 sy, i32 sw, i32 sh, bool flipx, bool flipy) {
		if (spr == nullptr) {
			LogE("Invalid sprite.");
			return;
		}
		i32 w = sw > 0 ? sw : spr->width();
		i32 h = sh > 0 ? sh : spr->height();

		for (u32 iy = 0; iy < h; iy++) {
			for (u32 ix = 0; ix < w; ix++) {
				i32 px = ix + x;
				i32 py = iy + y;

				i32 srcx = flipx ? w - 1 - ix : ix;
				i32 srcy = flipy ? h - 1 - iy : iy;

				Color col = spr->get(srcx + sx, srcy + sy);
				dot(px, py, col.r, col.g, col.b);
			}
		}
	}

	void Mik::aspr(Sprite* spr, i32 x, i32 y, bool flipx, bool flipy) {
		if (spr == nullptr) {
			LogE("Invalid sprite.");
			return;
		}
		tile(spr, spr->animator().frame(), x, y, flipx, flipy);
	}

	void Mik::tile(Sprite* spr, u32 index, i32 x, i32 y, bool flipx, bool flipy) {
		if (spr == nullptr) {
			LogE("Invalid sprite.");
			return;
		}
		i32 sw = std::floor(spr->width() / spr->cols());
		i32 sh = std::floor(spr->height() / spr->rows());
		i32 sx = (index % spr->cols()) * sw;
		i32 sy = std::floor(index / spr->cols()) * sh;
		this->spr(spr, x, y, sx, sy, sw, sh, flipx, flipy);
	}

	i32 Mik::chr(Sprite* fnt, std::string const& charMap, char c, i32 x, i32 y) {
		if (fnt == nullptr) {
			LogE("Invalid font.");
			return -1;
		}
		i32 cw = std::floor(fnt->width() / fnt->cols());

		u32 index = charMap.find_first_of(c);
		tile(fnt, index, x, y);

		return x + cw;
	}

	void Mik::text(Sprite* fnt, std::string const& charMap, std::string const& text, i32 x, i32 y) {
		if (fnt == nullptr) {
			LogE("Invalid font.");
			return;
		}
		i32 tx = x;
		i32 ty = y;
		i32 ch = std::floor(fnt->height() / fnt->rows());

		for (size_t i = 0; i < text.size(); i++) {
			char c = text.at(i);
			if (c == '\n') {
				tx = x;
				ty += ch + 1;
			} else {
				tx = chr(fnt, charMap, c, tx, ty);
			}
		}
	}

	Sprite* Mik::loadSprite(std::string const& fileName, u32 rows, u32 cols) {
		if (fileName.empty()) {
			LogE("Invalid file name.");
			return nullptr;
		}
		m_sprites.push_back(std::make_unique<Sprite>(fileName));
		m_sprites.back().get()->rows(rows);
		m_sprites.back().get()->cols(cols);
		return m_sprites.back().get();
	}

	Sprite* Mik::createSprite(u32 w, u32 h) {
		if (w == 0 || h == 0) {
			LogE("Invalid dimensions.");
			return nullptr;
		}
		m_sprites.push_back(std::make_unique<Sprite>(w, h));
		return m_sprites.back().get();
	}

	Sound* Mik::loadSound(std::string const& fileName) {
		m_sounds.push_back(std::make_unique<Sound>(fileName));
		return m_sounds.back().get();
	}

}