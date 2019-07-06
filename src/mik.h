#ifndef MIK_H
#define MIK_H

#include "sprite.h"
#include "animator.h"
#include "audio.h"

#include <memory>
#include <array>

#if !__has_include("SDL.h")
#include "SDL2/SDL.h"
#else
#include "SDL.h"
#endif

namespace mik {
	constexpr u32 MikScreenWidth = 320;
	constexpr u32 MikScreenHeight = 240;
	constexpr u32 MikScreenUpscale = 2;
	constexpr f32 MikTimeStep = 1.0 / 60.0;

	enum MikEvent {
		/// "Joystick" Events
		MikButtonLeft = 0,
		MikButtonRight,
		MikButtonUp,
		MikButtonDown,
		MikButtonStart,
		MikButtonA,
		MikButtonB,

		/// Mouse Events
		MikMouseLeft,
		MikMouseMiddle,
		MikMouseRight,

		MikEventCount
	};

	class Mik;
	class MikGame {
	public:
		MikGame() = default;
		virtual ~MikGame() = default;

		virtual void onInit(Mik& mik) {}
		virtual void onUpdate(Mik& mik, float timeStep) {}
		virtual void onDraw(Mik& mik) {}
		virtual void onExit(Mik& mik) {}
	};

	/// The MIK engine
	/// MIK stands for Mikro
	class Mik {
	public:
		virtual ~Mik() = default;
		Mik(MikGame* game);
		void run();

		/// Quits
		void quit() { m_running = false; }

		/// Returns if any of left, right, up, down, a, b, left_mouse, middle_mouse, right_mouse is pressed.
		bool buttonPressed(std::string const& name);

		/// Returns if any of left, right, up, down, a, b, left_mouse, middle_mouse, right_mouse is released.
		bool buttonReleased(std::string const& name);

		/// Returns if any of left, right, up, down, a, b, left_mouse, middle_mouse, right_mouse is held.
		bool buttonHeld(std::string const& name);

		/// Warps mouse to X, Y
		void warpMouse(i32 x, i32 y);

		/// Gets mouse X coord
		i32 mouseX() const { return m_mouseX; }

		/// Gets mouse Y coord
		i32 mouseY() const { return m_mouseY; }

		/// Gets the window title
		std::string title() const;

		/// Sets the window title
		void setTitle(std::string const& title);

		/// Sets the key color (for transparency)
		void key(u8 r, u8 g, u8 b) { m_key = { r, g, b, false }; }

		/// Sets the clipping region
		void clip(u32 x, u32 y, u32 w, u32 h);

		/// Unsets the clipping region
		void unclip();

		/// Clears the screen to a specified color
		void clear(u8 r, u8 g, u8 b);

		/// Draws a pixel (dot)
		void dot(i32 x, i32 y, u8 r, u8 g, u8 b);

		/// Draws a line
		void line(i32 x1, i32 y1, i32 x2, i32 y2, u8 r, u8 g, u8 b);

		/// Draws a rectangle
		void rect(i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b);

		/// Draws a filled rectangle
		void rectf(i32 x, i32 y, i32 w, i32 h, u8 r, u8 g, u8 b);

		/// Draws a circle
		void circ(i32 x1, i32 y1, i32 radius, u8 r, u8 g, u8 b);

		/// Draws a filled circle
		void circf(i32 x1, i32 y1, i32 radius, u8 r, u8 g, u8 b);

		/// Draws a sprite
		void spr(
			Sprite* spr, i32 x, i32 y,
			i32 sx = 0, i32 sy = 0, i32 sw = 0, i32 sh = 0,
			bool flipx = false, bool flipy = false
		);

		/// Draws a tile
		void tile(
			Sprite* spr, u32 rows, u32 cols, u32 index, i32 x, i32 y,
			bool flipx = false, bool flipy = false
		);

		/// Draws a character. Returns the X coord of the next char.
		i32 chr(Sprite* fnt, std::string const& charMap, char c, i32 x, i32 y);

		/// Draws a string.
		void text(Sprite* fnt, std::string const& charMap, std::string const& text, i32 x, i32 y);

		/// Loads a sprite (Sprite)
		Sprite* loadSprite(std::string const& fileName);

		// Creates a sprite (Sprite)
		Sprite* createSprite(u32 w, u32 h);

		/// Loads a Sound
		Sound* loadSound(std::string const& fileName);

		/// Audio System
		MikAudio* audio() { return m_audio.get(); }

	private:
		SDL_Window* m_window;
		SDL_Renderer* m_renderer;
		SDL_Texture* m_buf;

		bool m_running{ false };

		/// GFX
		Sprite m_buffer;
		u32 m_clip[4];
		Color m_key;
		///

		/// Assets
		std::vector<std::unique_ptr<Sprite>> m_sprites;
		std::vector<std::unique_ptr<Sound>> m_sounds;
		///

		std::unique_ptr<MikGame> m_game;

		/// Audio
		std::unique_ptr<MikAudio> m_audio;
		SDL_AudioDeviceID m_device;
		///

		struct EventState {
			bool pressed{ false }, released{ false }, held{ false };
		};
		using InputDevice = std::array<EventState, MikEventCount>;

		InputDevice m_input;
		i32 m_mouseX, m_mouseY;

		/// Helpers
		bool pointInClip(i32 x, i32 y) const;
		bool clipEnabled() const;
	};
}

#endif // MIK_H