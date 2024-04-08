#!/usr/bin/bash

# Remove unnecessary stuff
sudo systemctl disable cups
sudo apt purge -y wolfram-engine libreoffice* scratch* sonic-pi cups chromium-browser vlc geany thonny
sudo apt autoremove -y

sudo apt-get update -y
sudo apt-get upgrade -y

# Install Polvo and Midi Hub dependencies
sudo apt-get install git ruby portaudio19-dev libsndfile1 ffmpeg python3-tk python3-gi gir1.2-gtk-3.0 libblas-dev -y

## Install Python 3.9
curl https://pyenv.run | bash
pyenv install 3.9
pyenv global 3.9

## Install tensorflow
curl -L https://github.com/PINTO0309/Tensorflow-bin/releases/download/v2.9.0/tensorflow-2.9.0-cp39-none-linux_aarch64.whl -o tensorflow-2.9.0-cp39-none-linux_aarch64.whl
pip install tensorflow-2.9.0-cp39-none-linux_aarch64.whl

## Install python requirements
pip install librosa==0.10.1 llvmlite-0.42.0 numba==0.59.1 numpy==1.26.4 spleeter PyAudio pydub pycairo PyGObject


# Install Midi Hub
git clone https://github.com/daniel-hartmann/pi-midi-host
cd pi-midi-host

## Optimize for power efficiency and fast boot
# sudo cp cmdline.txt /boot/

## Install MIDI autoconnect script
sudo cp connectall.rb /usr/local/bin/
sudo cp 33-midiusb.rules /etc/udev/rules.d/
sudo udevadm control --reload
sudo service udev restart
sudo cp midi.service /lib/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable midi.service
sudo systemctl start midi.service

## FW for older Midisport devices
sudo apt-get install midisport-firmware -y

## Setup MIDI bluetooth
git clone https://github.com/oxesoft/bluez
sudo apt-get install -y autotools-dev libtool autoconf
sudo apt-get install -y libasound2-dev
sudo apt-get install -y libusb-dev libdbus-1-dev libglib2.0-dev libudev-dev libical-dev libreadline-dev
cd bluez
./bootstrap
./configure --enable-midi --prefix=/usr --mandir=/usr/share/man --sysconfdir=/etc --localstatedir=/var
make
sudo make install
cd ..
sudo cp 44-bt.rules /etc/udev/rules.d/
sudo udevadm control --reload
sudo service udev restart
sudo cp btmidi.service /lib/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable btmidi.service
sudo systemctl start btmidi.service

## Create alias to show connected devices
echo >> ~/.bashrc
echo "alias midi='aconnect -l'" >> ~/.bashrc
echo >> ~/.bashrc

## Create alias to reconnect devices
echo >> ~/.bashrc
echo "alias connect='connectall.rb'" >> ~/.bashrc
echo >> ~/.bashrc


# Install Polvo
cd /opt
sudo git clone https://github.com/daniel-hartmann/polvo.git
cd polvo
cp polvo.desktop ~/.config/autostart
# sudo cp config.txt /boot/

## Make device identifiable more easily on the network
sudo apt-get install avahi-daemon -y
sudo sed -i -- 's/raspberrypi/polvo/g' /etc/hostname /etc/hosts
sudo hostname polvo


read -p "Press [enter] to reboot"
sudo reboot