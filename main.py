#!/usr/bin/env python3

import pyaudio

import gui


def list_audio_devices():
    p = pyaudio.PyAudio()
    for i in range(p.get_device_count()):
        device_info = p.get_device_info_by_index(i)
        print(f"Device {i}: {device_info['name']}")


if __name__ == "__main__":
    gui.init()
    # list_audio_devices()
