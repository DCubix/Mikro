.. _api:

Mikro's Lua API
===============

The `mik` module wraps every functionality
from Mikro's C++ API to the Lua language.

Make sure you've read the :ref:`GettingStarted` guide.

.. function:: mik.quit():

    Quits the game.

.. function:: mik.btnp(name):

    Returns true when a button has been pressed.

    :type name: string
    :arg name: The button name. :ref:`buttons`.

.. function:: mik.btnr(name):

    Returns true when a button has been released.

    :type name: string
    :arg name: The button name. :ref:`buttons`.

.. function:: mik.btnh(name):

    Returns true when a button has been held.

    :type name: string
    :arg name: The button name. :ref:`buttons`.

.. function:: mik.warp(x, y):

    Warps the mouse to a specific location.

    :type x: number
    :arg x: X coordinate.
    :type y: number
    :arg y: Y coordinate.

.. function:: mik.mousex():

    Gets the current mouse's X coordinate.

.. function:: mik.mousey():

    Gets the current mouse's Y coordinate.

.. function:: mik.title([text]):

    Gets/Sets the window title.
    If no argument is specified, it acts as a getter.

    :type text: string
    :arg text:  The text to set the window title to (optional).
    :rtype: string
    :return: The window title.

.. function:: mik.key(r, g, b):

    Sets the transparency key color.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.clip([x, y, w, h]):

    Sets/Unsets the clipping region of the next drawing commands.
    If no argument is passed, or if any of the width and height components is zero,
    clipping will be disabled.

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.
    :type w: number
    :arg w: Width component.
    :type h: number
    :arg h: Height component.

.. function:: mik.clear([r, g, b]):

    Clears the screen to an optional background color.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.dot(x, y, r, g, b):

    Draws a `dot` (pixel) at a specified `position` (x, y) with a specified `color` (r, g, b).

    :type x: number
    :arg x: X position component.
    :type y: number
    :arg y: Y position component.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.line(x1, y1, x2, y2, r, g, b):

    Draws a `line` between two point with a specified `color` (r, g, b).

    :type x1: number
    :arg x1: Start X position component.
    :type y1: number
    :arg y1: Start Y position component.
    :type x2: number
    :arg x2: End X position component.
    :type y2: number
    :arg y2: End Y position component.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.rect(x, y, w, h, r, g, b):

    Draws a `rectangle` with a specified `color` (r, g, b).

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.
    :type w: number
    :arg w: Width component.
    :type h: number
    :arg h: Height component.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.rectf(x, y, w, h, r, g, b):

    Draws a `filled rectangle` with a specified `color` (r, g, b).

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.
    :type w: number
    :arg w: Width component.
    :type h: number
    :arg h: Height component.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.circ(x, y, radius, r, g, b):

    Draws a `circle` with a specified `color` (r, g, b).

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.
    :type radius: number
    :arg radius: Radius.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.circf(x, y, radius, r, g, b):

    Draws a `filled circle` with a specified `color` (r, g, b).

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.
    :type radius: number
    :arg radius: Radius.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

.. function:: mik.spr(sprite, x, y, [sx, sy, sw, sh, flipx, flipy]):

    Draws a `sprite`.

    :type sprite: Sprite object
    :arg sprite: A sprite.

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

    :type sx: number
    :arg sx: Source rect X component (optional).
    :type sy: number
    :arg sy: Source rect Y component (optional).
    :type sw: number
    :arg sw: Source rect Width component (optional).
    :type sh: number
    :arg sh: Source rect Height component (optional).

    :type flipx: bool
    :arg flipx: If true, flips the sprite horizontally (optional).
    :type flipy: bool
    :arg flipy: If true, flips the sprite vertically (optional).

.. function:: mik.aspr(sprite, x, y, [flipx, flipy]):

    Draws an `animated sprite`.

    :type sprite: Sprite object
    :arg sprite: A sprite.

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

    :type flipx: bool
    :arg flipx: If true, flips the sprite horizontally (optional).
    :type flipy: bool
    :arg flipy: If true, flips the sprite vertically (optional).

.. function:: mik.tile(sprite, index, x, y, [flipx, flipy]):

    Draws a `sprite tile`.

    :type sprite: Sprite object
    :arg sprite: A sprite.

    :type index: number
    :arg index: Tile index.

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

    :type flipx: bool
    :arg flipx: If true, flips the sprite horizontally (optional).
    :type flipy: bool
    :arg flipy: If true, flips the sprite vertically (optional).

.. function:: mik.chr(font, map, char, x, y):

    Draws a single `character`.

    :type font: Sprite object
    :arg font: Font sprite.

    :type map: string
    :arg map: Character map string.

    :type char: string
    :arg char: Character to draw.

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

.. function:: mik.text(font, map, text, x, y):

    Draws a `string`.

    :type font: Sprite object
    :arg font: Font sprite.

    :type map: string
    :arg map: Character map string.

    :type text: string
    :arg text: Text.

    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

.. function:: mik.load_sprite(fileName, [rows, cols]):

    Loads a `sprite` from a PNG image file.

    :type fileName: string
    :arg fileName: PNG image file name.

    :type rows: int
    :arg rows: Number of vertical sprites in the sprite sheet (optional).
    :type cols: int
    :arg cols: Number of horizontal sprites in the sprite sheet (optional).

    :rtype: Sprite object
    :return: A sprite.

.. function:: mik.create_sprite(w, h):

    Creates an empty `sprite`.

    :type w: number
    :arg w: Sprite width.
    :type h: number
    :arg h: Sprite height.

    :rtype: Sprite object
    :return: A sprite.

.. function:: mik.width([sprite]):

    Gets the width of a `sprite` or the width of the `screen` if no parameter is specified.

    :type sprite: Sprite object
    :arg sprite: A sprite.

.. function:: mik.height([sprite]):

    Gets the height of a `sprite` or the height of the `screen` if no parameter is specified.

    :type sprite: Sprite object
    :arg sprite: A sprite.

.. function:: mik.pget(sprite, x, y):

    Gets a pixel at a certain position from a `sprite`.

    :type sprite: Sprite object
    :arg sprite: A sprite.
    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

    :rtype: table {r, g, b}
    :return: The pixel color.

.. function:: mik.pset(sprite, x, y, r, g, b, [ghost]):

    Sets a pixel at a certain position in a `sprite`.

    :type sprite: Sprite object
    :arg sprite: A sprite.
    :type x: number
    :arg x: X component.
    :type y: number
    :arg y: Y component.

    :type r: number
    :arg r: Red component.
    :type g: number
    :arg g: Green component.
    :type b: number
    :arg b: Blue component.

    :type ghost: bool
    :arg ghost: If true, the pixel will be marked as a "ghost pixel",
        in other words, it will be ignored by the renderer, making it "transparent".

.. function:: mik.frame(sprite):

    Gets the current frame from a sprite.

    :type sprite: Sprite object
    :arg sprite: A sprite.

.. function:: mik.animation(sprite):

    Gets the current animation name from a sprite.

    :type sprite: Sprite object
    :arg sprite: A sprite.

.. function:: mik.play(sprite, name, speed, loop):

    Plays a sprite animation.

    :type sprite: Sprite object
    :arg sprite: A sprite.
    :type name: string
    :arg name: Animation name.
    :type speed: number
    :arg speed: Animation speed in seconds.
    :type loop: bool
    :arg loop: Whether or not the animation should loop.

.. function:: mik.add_animation(sprite, name, [frames]):

    Adds an animation to a sprite.

    :type sprite: Sprite object
    :arg sprite: A sprite.
    :type name: string
    :arg name: Animation name.
    :type frames: string
    :arg frames: Animation frames array (optional).

.. function:: mik.load_sound(fileName):

    Loads a `sound` from a WAV file.

    :type fileName: string
    :arg fileName: WAV file name.

    :rtype: Sound object
    :return: A sound.

.. function:: mik.play_sound(sound, [gain, pitch, pan]):

    Plays a `sound`.

    :type sound: Sound object
    :arg sound: A sound.

    :type gain: number
    :arg gain: The initial gain (volume) (optional).
    :type pitch: number
    :arg pitch: The initial pitch (optional).
    :type pan: number
    :arg pan: The initial pan (optional).

    :rtype: Voice object
    :return: The voice this sound is going to be played in.

.. function:: mik.play_music(sound, [gain, pan, fade]):

    Plays a `music`.

    :type sound: Sound object
    :arg sound: A sound.

    :type gain: number
    :arg gain: The initial gain (volume) (optional).
    :type pan: number
    :arg pan: The initial pan (optional).
    :type fade: number
    :arg fade: Fade time in seconds (optional).

.. function:: mik.stop_music([fade]):

    Stops the current `music`.

    :type fade: number
    :arg fade: Fade time in seconds (optional).

.. function:: mik.volume(voice, [value]):

    Gets/Sets the voice's volume/gain.
    If no `value` argument is passed, it will act as a getter.

    :type voice: Voice object
    :arg voice: A voice from **mik.play_sound**.
    :type value: number
    :arg value: The volume/gain value [0.0 - 1.0].

.. function:: mik.master_volume([value]):

    Gets/Sets the master volume/gain.
    If no `value` argument is passed, it will act as a getter.

    :type value: number
    :arg value: The volume/gain value [0.0 - 1.0].

.. function:: mik.pitch(voice, [value]):

    Gets/Sets the voice's pitch.
    If no `value` argument is passed, it will act as a getter.

    :type voice: Voice object
    :arg voice: A voice from **mik.play_sound**.
    :type value: number
    :arg value: The pitch value [0.1 - 10.0].

.. function:: mik.pan(voice, [value]):

    Gets/Sets the voice's pan.
    If no `value` argument is passed, it will act as a getter.

    :type voice: Voice object
    :arg voice: A voice from **mik.play_sound**.
    :type value: number
    :arg value: The pan value [-1.0 - 1.0].

.. _exts:

Math Library Extensions
"""""""""""""""""""""""

.. function:: math.lerp(a, b, t):

    Linear interpolation.

.. function:: math.clamp(v, min, max):

    Value clamping.

.. _buttons:

Button Constants
""""""""""""""""

.. data:: left

    Left button. Mapped to **Left Arrow Key** and **S Key**.

.. data:: right

    Right button. Mapped to **Right Arrow Key** and **F Key**.

.. data:: up

    Up button. Mapped to **Up Arrow Key** and **E Key**.

.. data:: down

    Down button. Mapped to **Down Arrow Key** and **D Key**.

.. data:: a

    A button. Mapped to **Z Key** and **K Key**.

.. data:: b

    B button. Mapped to **X Key** and **L Key**.

.. data:: start

    Start button. Mapped to **Enter Key** and **Numpad Enter Key**.

.. data:: left_mouse

    Left mouse button.

.. data:: middle_mouse

    Middle mouse button.

.. data:: right_mouse

    Right mouse button.
