import datetime
import os
import threading

import wave
import numpy as np
import pyaudio
from pydub import AudioSegment
from pydub.playback import play
from spleeter.separator import Separator

# Constants
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
CHUNK = 1024
SILENCE_THRESHOLD = 2.5  # Adjust according to your environment
SILENCE_DURATION = 3  # in seconds

# Separator
INSTRUMENTS = ("vocals", "drums", "bass", "piano", "other")
NUMBER_OF_STEMS = 5
OUTPUT_DIR = "stems/"


class Polvo:
    def __init__(self, status_callback=None):
        self.filename = ""
        self.status_callback = status_callback
        self.user_stopped = False
        self.stems_created = False

    def set_status(self, text):
        if self.status_callback:
            self.status_callback(text)

    def stop(self):
        self.user_stopped = True

    def play_last_recording(self):
        if self.filename:
            audio = AudioSegment.from_file(self.filename)
            play(audio)

    def listen(self):
        self.cleanup()

        audio = pyaudio.PyAudio()
        default_index = audio.get_default_input_device_info()["index"]

        stream = audio.open(
            format=FORMAT,
            channels=CHANNELS,
            rate=RATE,
            input=True,
            frames_per_buffer=CHUNK,
            input_device_index=default_index,
        )

        self.filename = ""
        silence_count = 0
        recording = False
        frames = []

        print("Listening...")
        self.set_status("Listening...")

        while True:
            data = stream.read(CHUNK)
            frames.append(data)

            if not recording and not self.is_silence(
                np.frombuffer(data, dtype=np.int16)
            ):
                self.set_status("Recording started.")
                recording = True

            if self.user_stopped or (
                recording and self.is_silence(np.frombuffer(data, dtype=np.int16))
            ):
                silence_count += 1
                if (
                    self.user_stopped
                    or silence_count >= RATE / CHUNK * SILENCE_DURATION
                ):
                    self.set_status("Recording stopped.")

                    stream.stop_stream()
                    stream.close()
                    audio.terminate()

                    self.filename = self.create_filename()

                    self.set_status(f"Saving file {self.filename}")
                    # Save the recorded audio as WAV
                    try:
                        # Save the recorded audio as WAV
                        with wave.open(self.filename, "wb") as wf:
                            wf.setnchannels(CHANNELS)
                            wf.setsampwidth(audio.get_sample_size(FORMAT))
                            wf.setframerate(RATE)
                            wf.writeframes(b"".join(frames))
                    except Exception as e:
                        print("Error occurred while saving the file:", e)

                    self.separate()
                    break
            else:
                silence_count = 0

    def separate(self):
        self.set_status(f"Creating {NUMBER_OF_STEMS} stems...")
        separator = Separator(f"spleeter:{NUMBER_OF_STEMS}stems", multiprocess=False)
        separator.separate_to_file(self.filename, OUTPUT_DIR)
        self.stems_created = True
        self.set_status("Stems created.")

    def is_silence(self, data):
        # Calculate RMS (root mean square) to determine if the input is silent
        rms = np.sqrt(np.mean(np.square(data)))
        if rms >= SILENCE_THRESHOLD:
            print(f"rms: {rms}")
        return rms < SILENCE_THRESHOLD

    def play_instrument(self, instrument):
        self.set_status(f"Playing {instrument}")
        filename_str = os.path.splitext(os.path.basename(self.filename))[0]
        output_file = f"{OUTPUT_DIR}{filename_str}/{instrument}.wav"
        audio = AudioSegment.from_file(output_file)
        play(audio)
        self.set_status("Ready")

    def cleanup(self):
        self.user_stopped = False
        try:
            # Remove the output files
            for instrument in INSTRUMENTS:
                filename_str = os.path.splitext(os.path.basename(self.filename))[0]
                output_file = f"{OUTPUT_DIR}{filename_str}/{instrument}.wav"
                os.remove(output_file)

            # Remove the original file
            os.remove(self.filename)
        except Exception:
            pass

    def create_filename(self):
        return datetime.datetime.now().strftime("%Y%m%d%H%M%S") + ".wav"
