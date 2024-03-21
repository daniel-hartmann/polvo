

## Dependencies

```
sudo apt-get install portaudio19-dev libsndfile1 ffmpeg python3-tk python3-gi gir1.2-gtk-3.0
pip install pyaudio numpy spleeter pydub pycairo PyGObject
```


```
sudo cp polvo.service /etc/systemd/system/
sudo systemctl enable polvo.service
sudo systemctl start polvo.service
```


