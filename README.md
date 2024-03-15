# stepper2
Second version of the lums stepper

Compiling in STM32CubeIDE:
1. go to stepper2.ioc and execute "Generate Code"
2. go to Project -> Properties -> C/C++ Build -> Settings
  - in MCU Settings check both "Use float..." options
  - in "MCU Post build outputs" enable binary file (if needed)
  - in MCU GCC Compiler -> Optimization  change level to -O3
  - in MCU G++ Compiler -> Optimization  change level to -O3
  - If required, change configuration to Release in the top of the Settings window, and repeat last four steps
