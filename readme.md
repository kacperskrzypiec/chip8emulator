# CHIP-8 Emulator

This project was written for fun to run [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) games, and to try out the newest release of [SDL](https://github.com/libsdl-org/SDL).

To boot a ROM file, simply drag and drop it into the window. Pressing ESC pauses the emulator and displays the controls (F1-F6).

The file [newtetris.ch8](data/newtetris.ch8) in the *data* directory is a recompiled version of [Tetris [Fran Dachille, 1991].ch8](https://github.com/kripod/chip8-roms/blob/master/games/Tetris%20%5BFran%20Dachille%2C%201991%5D.ch8) using the (dis)assembler I wrote (coming soon to Github). I reverse-engineered the disassembled code and made several enhancements: the score counter is always visible, controls have been changed, sounds have been added for when a tetromino is placed and when a line is cleared.

## License

[MIT License](LICENSE)