#include "keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept {

	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept {

	if (keybuffer.size() > 0u) {

		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else {

		return Keyboard::Event();
	}

}

