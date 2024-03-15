# stepper2
Second version of the lums stepper

Compiling in STM32CubeIDE:
1. go to stepper2.ioc and execute "Generate Code"
1. go to Project -> Properties -> C/C++ Build -> Settings
  - in "MCU Post build outputs" enable binary file (if needed)
  - in MCU GCC Compiler -> Optimization  change level to -O3
  - in MCU G++ Compiler -> Optimization  change level to -O3