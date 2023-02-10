#!/bin/bash

sudo apt-get update
sudo apt-get upgrade

sudo apt-get install build-essential cmake pkg-config libjpeg-dev libtiff5-dev libjasper-dev libpng-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libfontconfig1-dev libcairo2-dev libgdk-pixbuf2.0-dev libpango1.0-dev libgtk2.0-dev libgtk-3-dev libatlas-base-dev gfortran libhdf5-dev libhdf5-serial-dev libhdf5-103 python3-pyqt5 python3-dev -y

sudo pip3 install opencv-python libwebp6

sudo apt install tesseract-ocr

sudo apt install libtesseract-dev

sudo pip3 install pytesseract

sudo pip3 install numpy --upgrade

sudo apt install bluetooth bluez blueman

sudo pip3 install pybluez

sudo pip3 install opencv-python