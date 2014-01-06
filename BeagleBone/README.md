# BeagleBone Black Setup

These instructions let you setup a BeagleBone Black (BBB) for potential use as the CPU for the Themostat.  It is a work in progress.

The goal would be to have the  (BBB) be the CPU for the unit without requiring an SD card.  These instructions will give steps to
install Ubuntu on a 4GB SD card and boot from it instead of the internal flash.  If we decide to use this board and get closer
to it working, we will figure out how to make the code fit on board and have instructions on how to do that.

# Steps to get it going

## Stuff to buy

1.  Get a BBB board.  [http://www.adafruit.com/products/1278](http://www.adafruit.com/products/1278)

2.  Get a 4GB Micro SD card.  [http://www.adafruit.com/products/102](http://www.adafruit.com/products/102)

3.  Get a 5V 2A power supply.  [http://www.adafruit.com/products/276](http://www.adafruit.com/products/276)
4.  Get a temperature sensor. Because of the good support for the One-Wire protocol on the Beagle Bone Black, we have chosen to use the Dallas 18b20 sensor. Note that these can be had for ~$5, but we used one from Adafruit that is waterproof and in a better package than the base component. [http://www.adafruit.com/products/381](http://www.adafruit.com/products/381)

## Download

1.  Download the Ubuntu 13.10 image [http://s3.armhf.com/debian/saucy/bone/ubuntu-saucy-13.10-armhf-3.8.13-bone30.img.xz](http://s3.armhf.com/debian/saucy/bone/ubuntu-saucy-13.10-armhf-3.8.13-bone30.img.xz)

2.  Uncompress file using Unarchiver on Mac OS X, 7-Zip on Windows or xz on Linux

3.  Copy the image file to the SD card using dd on Mac OS X or Linux or an image writing program on Windows.  See this page
    for more info on how to do this:  [http://learn.adafruit.com/beaglebone-black-installing-operating-systems](http://learn.adafruit.com/beaglebone-black-installing-operating-systems)

## Boot the board

NOTE:  You may not need to hold down the button in step 4.

1.  Make sure the BBB is unplugged

2.  Put the micro SD card in the socket

3.  Connect an ethernet cable to the BBB

4.  Hold down the button identified in this image [http://learn.adafruit.com/beaglebone-black-installing-operating-systems/flashing-the-beaglebone-black](http://learn.adafruit.com/beaglebone-black-installing-operating-systems/flashing-the-beaglebone-black)

5.  Connect power to the board until the LEDS light for a few seconds then release

6.  Wait 30 seconds or so for it to boot

## Login to the board

1.  You will need to determine the IP address of the BBB board.  How to do that depends on your router.  Once you find the IP address, you then ssh into
    the board using the username **ubuntu** to that IP address.

2.  The password is **ubuntu**.

## Update the software

1.  You need to update the software on the board.  Run these commands:
    1.  **sudo apt-get update**
    2.  **sudo apt-get upgrade**

## Setup the web server

The self signed SSL certificate was created using these directions: [http://www.akadia.com/services/ssh_test_certificate.html](http://www.akadia.com/services/ssh_test_certificate.html).  Since the key is checked into github, this certificate should not be used in a production system.

1.  Run **sudo apt-get install lighttpd**

2.  Copy files from **files** in this folder into the same folders on the BBB.

3.  Restart the web server by running **sudo service lighttpd restart**

4.  You can then test this by using your browser and going to **https://*ipaddress*/cgi-bin/go.py**

## Setup the Temperature Sensor
Instructions here are based on [http://hipstercircuits.com/dallas-one-wire-temperature-reading-on-beaglebone-black-with-dto/](http://hipstercircuits.com/dallas-one-wire-temperature-reading-on-beaglebone-black-with-dto/). There were some things we needed to do that were not outlined on that page due to differences because we were using Ubuntu. Specifically:

Before the step where you compile the dts file, ensure you install the patched version of dtc like so:

**wget -c https://raw.github.com/RobertCNelson/tools/master/pkgs/dtc.sh 
chmod +x dtc.sh 
./dtc.sh**

dtc will be located at '/usr/local/bin/dtc' - ensure you are using this.

In the step where you add your device to 'slots', you need to use this syntax:
**sudo sh -c " echo BB-W1:00A0 > /sys/devices/bone_capemgr.9/slots"**
otherwise you'll get 'permission denied'.

The address of your device will be different from the one at the referenced link. Find your address by typing **ls /sys/bus/w1/devices/**

We experimented with the DHT-22 temperature and humidity sensor. It has an idiosyncratic protocol that does not quite match the One-Wire protocol, and though we found examples of people using C programs running on the Raspberry Pi or BeagleBone Black to read it, we found in practice these gave very flaky and unreliable readings, perhaps due to the fact that the code is being run concurrently with other processes on the system and timing can be off. We also noticed the DHT-22 would get into a state where it just stopped responding. We've had much better results with the DS18b20.
