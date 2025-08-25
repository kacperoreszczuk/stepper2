# stepper2
Second version of the lums stepper

Compiling in STM32CubeIDE:

1. Install STM32CubeIDE, sadly requires account creation
2. Click "Import project" and select "fw" directory from this repository
3. In the "Project Explorer" panel on the left open stepper2.ioc
    - confirm "open associated perspecitve" if prompted
    - continue with the current version of the firmware if prompted (do not migrate)
    - Login into MyST account if prompted
    - if not prompted and you get problems later in the instruction in point 5 (more precisely "this project is not a CDT project") then in the stepper2.ioc window click Help->Configuration Tool->Manage Embedded Software Libraries, scroll to STM32H7 and check tick box near version 1.11.1. Click Install. Hopefully you will be prompted to login and the package will install. Continue with instruction (starting here with Project->Generate Code)
    - in the top menu select Project->Generate Code
    - confirm "open associated perspecitve" if prompted
5. go to Project -> Properties -> C/C++ Build -> Settings
    - if "Properties" button is inactive click on main.c file subwindow to focus on it
    - in MCU(or MCU/MPU) Settings  check both "Use float..." options
    - in "MCU Post build outputs" enable binary file (or "convert to binary file")
    - in MCU(or MCU/MPU) GCC Compiler->Optimization: change level to -O3
    - in MCU(or MCU/MPU) G++ Compiler->Optimization: change level to -O3
    - If you want to switch to release configuration, change configuration to Release in the top of the Settings window, and repeat last four steps. In that case take care to build in the release later.
    - click apply and close, confirm if prompted to rebuild index
6. You may want to go to Core/Inc/defaults.h and change #define DRIVER_ID to a different value (any you like), especially if you would use more than one device simultaneously and be able to discern between them when talking through serial port.
7. Power up the driver and connect it with micro usb port (its near the RJ45 port). Click Run->Run or click the green arrow icon to build and flash the device.
    - You may be prompted to upgrade the stlink firmware. Agree. In the STLinkUpgrade Window click "Open in update mode" and then "Upgrade". Cycle power to the driver and try again number 7 in this instruction.
9. Alternatively to point 7. you can select Project->Build Project and then copy manually the file in Debug/stepper2.bin to the mass storage device that opens after conneting the driver. 
