# 🏡 Cozy Village

## 📋 1. Basic Info
* 📘 **Course:** XJEL2645 Embedded Systems Project
* 👥 **Group:** Group 18
* 🔖 **Version:** Rev 1.0 (Unit 4 Group Project)

## 👥 2. Team Members & Responsibilities
Each member took full ownership of their respective game module and shared system components:
* 👩‍💻 **Wanying Wang:** Game 1 (Harvest), physical hardware wiring, and system integration debugging.
* 👩‍💻 **Yuanxi Zhang:** Game 2 (Lake), global economy architecture, and shop interface implementation.
* 👩‍💻 **Ran Wei:** Game 3 (Mine), opening animation, main menu, and UART serial logging.

## 📖 3. Project Description
**Cozy Village** is a collaborative embedded games console built on the STM32 Nucleo-L476RG development board. The project features a unified world where players explore three independent activities—farming, fishing, and mining. All games are integrated into a single program with a shared menu and economy system running on a single STM32 Nucleo board.

## 🗂️ 4. Project Structure
The project uses a modular structure to isolate logic. Files are listed individually to align with standard repository conventions:

```text
CozyVillage/
├── Core/               # STM32 auto-generated files
├── Drivers/            # STM32 HAL drivers
├── cmake/              # CMake build configuration & toolchains
├── shared/             # Shared system resources
│   ├── Menu.c
│   ├── Menu.h
│   ├── InputHandler.c
│   ├── InputHandler.h
│   ├── Economy.c
│   └── Economy.h
├── game_1/             # Game 1: Farm
│   ├── Game_1.c
│   ├── Game_1.h
│   └── ...             # Internal farming logic & sensor drivers
├── game_2/             # Game 2: Lake (Fishing)
│   ├── Game_2.c
│   ├── Game_2.h
│   └── ...             # Internal fishing logic & hardware drivers
├── game_3/             # Game 3: Mine (Torchlight Miner)
│   ├── Game_3.c
│   ├── Game_3.h
│   └── ...             # Internal map generation & serial logging
├── ST7789V2_Driver_STM32L4/ # LCD Display drivers
├── Joystick/           # Hardware drivers (Joystick)
├── PWM/                # PWM LED control
├── Buzzer/             # Buzzer control
└── CMakeLists.txt      # Primary build script

```

## ✨ 5. Features

* 💰 **Shared Economy:** A shared gold system connects the Farm, Lake and Mine modules through one village-level economy.
* 🧩 **Modular Integration:** Each game is developed in its own folder and connected through shared menu, input and economy interfaces.
* 🎛️ **Hardware Interaction:** The system uses joystick input, buttons, LCD graphics, buzzer feedback and game-specific sensors or output devices.
* ⏱️ **Timed Gameplay Events:** TIM6/TIM7 support timing-based behaviour where required, while game modules use timing control for stable rendering and input response.
* 🏡 **Unified Village Menu:** A shared 2D village menu allows the player to select and enter each game module from one embedded games console.

## 🎮 6. Game Modules

* 🌾 **Game 1: FARM:** A farming simulation where players manage a plot to grow crops, influenced by environmental factors.
* 🎣 **Game 2: LAKE:** A fishing simulator featuring motion-based casting and tension-based reeling mechanics.
* ⛏️ **Game 3: MINE:** A procedural cave exploration game where players manage limited light resources while mining for ores.

## 🔌 7. Hardware

### Common Hardware

* 🧠 **STM32L476 Microcontroller:** Core processor of the embedded games console.
* 📺 **ST7789V2 LCD Display:** Primary graphical output (240×240).
* 🕹️ **Joystick:** Analogue directional input for movement and menu navigation.
* 🔘 **Buttons:** Additional digital inputs for selection, actions and returning to the menu.
* 🔊 **Buzzer:** Audio feedback and sound effects.
* 💡 **PWM / RGB LED:** Visual feedback and lighting effects.
* ⏱️ **Timers (TIM6 & TIM7):** Timing support for game events and system ticks.

### Game-Specific Hardware

* 🌡️ **Temperature Sensor:** Used in Farm to affect crop growth and water behaviour based on the environment.
* ☀️ **Light Sensor:** Used in Farm to control the day/night cycle logic.
* 🔄 **Potentiometer:** Provides analog control for specific farm mechanics.
* 🔢 **7-Segment Display:** Physically displays farm harvest counts or related statistics.
* 🤏 **Pressure Sensor:** Used in Lake for fishing tension mechanics.
* 📉 **Accelerometer:** Used in Lake to detect physical casting motions.
* 🖥️ **UART Serial Output:** Used in Mine for serial logging and terminal output.

## ⚙️ 8. Software Architecture

The system follows a **Hybrid State Machine** pattern. The main loop delegates execution to the current game module, which runs its own internal loop.

**Main Loop Delegation (main.c):**

```c
while(1) {
    switch(current_state) {
        case MENU_STATE_HOME:
            current_state = Menu_Run(&menu);
            break;
        case MENU_STATE_GAME_1:
            current_state = Game1_Run(); // Delegate to game runner
            break;
        // ... handles other games
    }
}

```

**Internal Game Runner Pattern (simplified pseudocode):**

```c
MenuState GameX_Run(void) {
    while (1) {
        Read_Input();           // Polling joystick and buttons
        if (exit_triggered) break; 
        
        Update_Game_Logic();    // Handle physics and state
        Render_Frame();         // Draw to LCD buffer
        LCD_Refresh(&cfg0);     // Push to screen
        
        HAL_Delay(FRAME_MS);    // Stable frame rate control
    }
    return MENU_STATE_HOME;     // Exit back to menu
}

```

## 🚀 9. Build and Run

1. Open the project in STM32CubeIDE.
2. Build the project for the STM32 Nucleo-L476RG target.
3. Flash the program to the Nucleo board.
4. Connect the LCD, joystick, buttons and required game-specific peripherals.
5. Use the Cozy Village main menu to enter Farm, Lake or Mine.

## 🔁 10. Shared Economy Flow

The shared `Economy` module provides a common gold interface for the three game modules. Farm, Lake and Mine can reward the player through gameplay and use the same village-level gold system for progression and shop interaction.
