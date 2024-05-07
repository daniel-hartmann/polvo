# UNDER REFACTOR

What is missing:
- compile the JUCE app in aarch64
- update setup.sh


# Polvo

Polvo is an interface for stem separation using Spleeter and MIDI routing using pi-midi-host.
It runs on Raspberry Pi 5. My idea is to have an extension hardware for the SP404 Mk2.

- [Spleeter](https://github.com/deezer/spleeter) by Deezer
- [JUCE](https://juce.com/get-juce/)
- [pi-midi-host](https://github.com/sinedied/pi-midi-host) by [Yohan Lasorsa](https://github.com/sinedied)


## Dependencies

Untested.

You'll need an USB Audio interface. Tested with:

- Roland SP404 MK2


## Installation

NOT WORKING
```
bash <(curl -Ls https://raw.githubusercontent.com/daniel-hartmann/polvo/main/setup.sh)
```
