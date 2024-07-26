# hp-omen-keyboard-temp-rgb
Daemon that changes the hue of your keyboard proportionally to the current system temperature on hp omen laptops.
## Dependencies
[**hp-omen-wmi-dkms** aur package](https://aur.archlinux.org/packages/hp-omen-wmi-dkms)
## Building
Run **build.sh** in the root of the project, change compile flags in **compile_flags.txt**
## Running
**keyboard-temp-rgb.service** is provided to run the daemon as a systemd service.
Otherwise, you can run the program as root in the background any other way and it will work.
