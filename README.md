<img width="1200" height="460" alt="GameME(1)" src="https://github.com/user-attachments/assets/13605cfa-c1c8-492f-b172-0ab05cc7289a" />


Ever wanted to make a game with a controller and play it
on your PC? Well this repo makes that dream come to life. I have set-ten up the controller and base of the game so you can customize everything. I used maybe a million commands along with tips to assist you. By the way i used Arduino IDE for the .ino code, for python i used VSC and for Godot i used Godot engine. STILL NOT TESTED ON ESP32.

# Table of contents:

- How to set up
- Wiring and file system
- Limitations --SEE THIS FIRST IF YOU USE WINDOWS

## How to set up:

Firstly we will start with customizing the `.ino` file.

<pre>
1. At the beginning, there is everything you need to customize. At the very bottom, there are  tips and tricks (shortcuts)
  and details if you want to fork. The comments are also there to help you fork.
  
2. Now to the both python file, No customization needed and there are comments for explanation
  
3. Now at the MainGame folder, in the Others folder, there is the `data.JSON`, MAKE SURE TO CHECK IF THE MAX ANALOG VALUE
  ON YOUR BOARD IS 1023 OR 4095.  The Script folder contains a `.gd` script. Thats for moving the sprite 
  based on the JSON. At the begining, it will explain what you need to do
  
4. After that, open terminal and write this line to execute server:
</pre>

```bash
cd <insert where the Games folder is>
bash run.sh
```

## Wiring and file system

### File pattern:
<pre>
Games/
|─ MainGame/       |I| This folder is where you can make your game using GoDot
│  ├─ Assets/      |I| Put the assets of your project here
│  ├─ Nodes/       |I| Put the scenes of your project here
│  ├─ Scripts/     |I| Put the codes here
│  └─ Others/      |I| Put other JSON files or stuff here
├─ Main.py         |I| Main entry point for the server
├─ Bridge.py       |I| Handles the internet connection part for Main.py
├─ README.md       |I| Tutorial file
├─ Games.ino       |I| File to upload to the ESP32
└─ run.sh          |I| Execute the server after setup
</pre>

### wiring
  
<img width="1587" height="2245" alt="wiring" src="https://github.com/user-attachments/assets/ce8b6f39-4b94-4a75-8e17-87fe29a00606" />
  where pin a = button a (like wise for a to f) k for joystick button (when u press DOWN on a joystick), x/y for dimentions of joystick movement



### OS:

Tested on Arch Linux, but can be compatible just the `run.sh` may need to be changed. Boards recomended: ESP32 dev-kit, ESP32-WROOM, similars

## Limitations:
This is specifically designed for Linux(arch) but can work on: MacOS, Debian-based linux distros, red-hat-based linux distros. To run on Windows listen the following instructions:

Instead to running the server by using `run.sh` you will use `run.bat`. Press win+R and type cmd to open terminal. then run this:

```cmd
cd <insert the FOLDER path>
run.bat
```
