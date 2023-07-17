# Basic Gamecube GPIO Driver

This is a basic driver to enable wiring the front panel of a gamecube to a raspberry pi via GPIO. The plan is to support getting the button presses and stick movements from the four ports, and to enable usage of the reset button with the Pi.

Progress
---

- [X] Serial I/O (Untested)
- [X] Probe for controllers (Untested)
- [X] Probe Origin (Untested)
- [X] Get Controller State (Untested)
- [ ] Implement timeouts for Serial I/O
- [ ] Convert State to Controller I/O on Linux
- [ ] Code to enable using the Reset Switch