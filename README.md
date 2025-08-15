<img width="1200" height="460" alt="GameME(1)" src="https://github.com/user-attachments/assets/13605cfa-c1c8-492f-b172-0ab05cc7289a" />


Ever wanted to make a game with a controller and play it
on your PC? Well this repo makes that dream come to life. I have set-ten up the controller and base of the game so you can customize everything. I used maybe a million commands along with tips to assist you. By the way i used Arduino IDE for the .ino code, for python i used VSC and for Godot i used Godot engine. STILL NOT TESTED ON ESP32.

# Table of contents:

- How to set up
- Details and facts

## How to set up:

Firstly we will start with customizing the .ino file.

At the beginning, there is everything you need to customize. At the very bottom, there are  tips and tricks (shortcuts) and details if you want to fork. The comments are also there to help you fork. Now to the both python file, At the beginning, there is a tutorial to help you. Now at the MainGame folder, In the others folder, there is the data.JSON, MAKE SURE TO CHECK IF THE MAX ADC ON YOUR BOARD IS 1023 OR 4095.  The Script folder contains a .gd script. Thats for moving the sprite based on the JSON. After that, open terminal and write this line:

```bash
chmod +x run.sh
# OR
cd <insert where the Games folder is>
bash run.sh
```

## Details and facts

### File pattern:
<pre>
Games/
|─ MainGame/       |/| This folder is where you can make your game using GoDot
│  ├─ Assets/      |/| Put the assets of your project here
│  ├─ Nodes/       |/| Put the scenes of your project here
│  ├─ Scripts/     |/| Put the codes here
│  └─ Others/      |/| Put other JSON files or stuff here
├─ Main.py         |/| Main entry point for the server
├─ Bridge.py       |/| Handles the internet connection part for Main.py
├─ README.md       |/| Tutorial file
├─ Games.ino       |/| File to upload to the ESP32
└─ run.sh          |/| Execute the server after setup


### wiring
  
<img width="1587" height="2245" alt="wiring" src="https://github.com/user-attachments/assets/ce8b6f39-4b94-4a75-8e17-87fe29a00606" />
  where pin a = button a (like wise for a to f) k for joystick button (when u press DOWN on a joystick), x/y for dimentions of joystick movement


</pre>
### OS:

Tested on Arch Linux, but can be compatible just the run.sh may need to be changed. RTOS: FreeRTOS. Boards recomended: ESP32 dev-kit, ESP32-WROOM, similars

