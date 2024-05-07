#!/usr/bin/bash

cd /opt
sudo chown -R $USER:$USER /opt


# Download stuff
git clone https://github.com/daniel-hartmann/polvo.git
wget https://github.com/juce-framework/JUCE/releases/download/7.0.12/juce-7.0.12-linux.zip
git clone https://github.com/daniel-hartmann/pi-midi-host
git clone https://github.com/oxesoft/bluez

# Remove unnecessary stuff
sudo systemctl disable cups
sudo apt purge -y wolfram-engine libreoffice* scratch* sonic-pi cups chromium-browser vlc geany thonny
sudo apt autoremove -y

sudo apt-get update -y
sudo apt-get upgrade -y

# Install Polvo and Midi Hub dependencies
sudo apt install git ffmpeg libsndfile1 avahi-daemon pkg-config libhdf5-dev -y
# For compiling Polvo
sudo apt install libasound2-dev libpulse-dev libwebkit2gtk-4.0-dev libgtk-3-dev juce-modules-source -y

## Extract JUCE
unzip -d polvo/ juce-7.0.12-linux.zip

## Install Python 3.9
if ! command -v pyenv &> /dev/null
then
    curl https://pyenv.run | bash
    echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.bashrc
    echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.profile
    echo 'command -v pyenv >/dev/null || export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.bashrc
    echo 'command -v pyenv >/dev/null || export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.profile
    echo 'eval "$(pyenv init -)"' >> ~/.bashrc
    echo 'eval "$(pyenv init -)"' >> ~/.profile
    . ~/.bashrc
    . ~/.profile

    sudo apt install build-essential libssl-dev zlib1g-dev \
        libbz2-dev libreadline-dev libsqlite3-dev curl \
        libncursesw5-dev xz-utils tk-dev libxml2-dev libxmlsec1-dev libffi-dev liblzma-dev -y

    pyenv install 3.9
    pyenv global 3.9
fi
echo "Python 3.9 installed"


# ## Install tensorflow
# curl -L https://github.com/PINTO0309/Tensorflow-bin/releases/download/v2.9.0/tensorflow-2.9.0-cp39-none-linux_aarch64.whl -o tensorflow-2.9.0-cp39-none-linux_aarch64.whl
# pip install --user --break-system-packages tensorflow-2.9.0-cp39-none-linux_aarch64.whl

## Install python requirements
pip install spleeter
# pip install librosa==0.10.1 llvmlite==0.42.0 numba==0.59.1 numpy==1.26.4 spleeter PyAudio pydub pycairo PyGObject


# Install Midi Hub

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
cd /opt/polvo/Polvo/Builds/LinuxMakefile
make



mkidir -p ~/.config/autostart
cp polvo.desktop ~/.config/autostart
# sudo cp config.txt /boot/

## Make device identifiable more easily on the network
sudo sed -i -- 's/raspberrypi/polvo/g' /etc/hostname /etc/hosts
sudo hostname polvo

## Change resolution
export DISPLAY=:0
resolution="800 480 60"

modename="POLVO_800x480_60"
display=$(xrandr | grep -Po '.+(?=\sconnected)')
xrandr --newmode $modename $(gtf $(echo $resolution) | grep -oP '(?<="\s\s).+')
xrandr --addmode $display $modename     
xrandr --output $display --mode $modename

systemctl --user enable pulseaudio
systemctl --user start pulseaudio

read -p "Press [enter] to reboot"
sudo reboot