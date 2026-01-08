# RoboRivals

Description

RoboRivals is a 2D robot battle game developed in C++ using the SFML library.
The game includes a weapon selection screen, a battle mode, background music,
sound effects, pickups, and simple physics like jumping and gravity.

---

SFML Version Used

* SFML version: 3.0.2
* Architecture: 64-bit
* Compiler/IDE: Visual Studio 2022
* SFML modules used:

  * sfml-graphics
  * sfml-window
  * sfml-system
  * sfml-audio

* How to Run the Game (Without Installing SFML) *

If you are running the provided executable:

1. Make sure all files stay in the same folder:

   * `.exe` file
   * SFML `.dll` files
   * Images (Robot sprites)
   * Audio files
   * Font file

2. Double-click the `.exe` file.
3. The game will run without installing SFML separately.

---

* How to Install SFML (For Compiling the Code)

-> Step 1: Download SFML

1. Go to the official SFML website: [https://www.sfml-dev.org](https://www.sfml-dev.org)
2. Download SFML 3.0.2 (64-bit) for Visual Studio 2022.
3. Extract the downloaded folder.

---

* Step 2: Set Up SFML in Visual Studio 2022

1. Open Visual Studio 2022.
2. Open the RoboRivals solution (`.sln` file).
3. Go to:

   * Project → Properties

## Include Directories

* Configuration Properties → C/C++ → General
* Add:

  ```
  path_to_sfml/include
  ```

## Library Directories

* Configuration Properties → Linker → General
* Add:

  ```
  path_to_sfml/lib
  ```

## Linker Input

* Configuration Properties → Linker → Input
* Add:

  ```
  sfml-graphics-d.lib
  sfml-window-d.lib
  sfml-system-d.lib
  sfml-audio-d.lib
  ```

---

# Step 3: DLL Files

Copy the following files from the SFML `bin` folder into the project folder (already done):

* sfml-graphics-d-2.dll
* sfml-window-d-2.dll
* sfml-system-d-2.dll
* sfml-audio-d-2.dll

---

# Controls

* U → Move selection up in weapon menu
* D → Move selection down in weapon menu
* R → Restart game from Game Over screen
* Movement & Shooting → Handled through keyboard input during battle:
-> 'A', 'D', 'W', for Robot 1 to move left, right and jump respectively.
-> 'Left arrow', 'Right arrow', 'Up arrow', for Robot 2 to move left, right and jump respectively.
---

# Game Structure

1. *Selection Mode* – Players choose weapons.
2. *Battle Mode* – Robots fight, shoot, jump, and collect pickups (orbs and magazines).
3. *Game Over Mode* – Game ends when health reaches zero, with restart and exit option.

---

# Notes

* All textures, audio, and font files must remain in the same directory as the executable.

# Credits

Developed by: Syeda Maira Faraz 
Library used: SFML (Simple and Fast Multimedia Library)
