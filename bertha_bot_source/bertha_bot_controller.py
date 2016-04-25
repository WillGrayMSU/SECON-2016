#!/usr/bin/env python
# .. -*- coding: utf-8 -*-
#
# ****************************************************************************
# bertha_bot_controller.py - The basic control class for the SECON 2016 robot
# ****************************************************************************
#
# Provides a RaspberryPi-to-Arduino interface for control functions
#
# Library imports
# ---------------
import spidev
import time
import os
import pigpio
import smbus
from PIL import Image


class RobotController(object):

    def __init__(self):

        # Setup SPI
        self.spi = spidev.SpiDev()

        # -------------------------------
        # GPIO pin for bump switch setup
        # -------------------------------
        self.pi1 = pigpio.pi()
        self.pi1.set_mode(18, pigpio.INPUT)
        cb1 = self.pi1.callback(18, pigpio.RISING_EDGE, self.bumpSwitch_ISR)
        self.bumpSwitchPressed = False

        #---------------------
        # Color sensing setup 
        #---------------------
        # Setup Color Sensor Array
        self.bus = smbus.SMBus(1)
        self.addr = 0x70
        self.addr1 = 0x29

        # Initialize and calibrate color sensors
        self.initializeColorSensors()

        # Approximate rail cart RGB arrays
        self.redCartColor = [91, 86, 17]
        self.blueCartColor = [74, 69, 2]
        self.greenCartColor = [111, 97, 16]
        self.yellowCartColor = [99, 94, 26]
        self.railCartColorMatrix = [self.redCartColor, self.blueCartColor, self.greenCartColor, self.yellowCartColor]


    # Sets the robot's drive command
    def SetNavCommand(self, command):
        self.spi.open(0,0)
        resp = self.spi.xfer2([command])
        #print resp
        self.spi.close()
        
    # Sets the robot's top platform command
    def SetSortCommand(self, command):
        self.spi.open(0,1)
        #resp = None
        resp = self.spi.xfer2([command])
        #print resp
        self.spi.close()

    # Bump switch interrupt service routine
    def bumpSwitch_ISR(self, gpio, level, tick):
        #print "Bump!"
        self.bumpSwitchPressed = True


    def initializeColorSensors(self):
        # Initialize color sensing
        try:
            self.bus.write_byte(self.addr, 0xFF)
            self.bus.write_byte(self.addr1,0x80|0x12)
            ver = self.bus.read_byte(self.addr1)
            self.bus.write_byte(self.addr1, 0x80 | 0x00)
            self.bus.write_byte(self.addr1, 0x01 | 0x02)
            self.bus.write_byte(self.addr1, 0x80 | 0x14)

            # Initialize color sensing values
            # Color Sensor Storage Arrays
            self.cleari = [0,0,0,0,0,0,0,0]
            self.redi = [0,0,0,0,0,0,0,0]
            self.greeni = [0,0,0,0,0,0,0,0]
            self.bluei = [0,0,0,0,0,0,0,0]

            self.clear = [0,0,0,0,0,0,0,0]
            self.red = [0,0,0,0,0,0,0,0]
            self.green = [0,0,0,0,0,0,0,0]
            self.blue = [0,0,0,0,0,0,0,0]
                
            # 0 = error
            # 1 = blue
            # 2 = green
            # 3 = red
            # 4 = yellow
            self.blockColors = [0, 0, 0, 0, 0, 0, 0, 0]
            self.sortedBlockColors = [0, 0, 0, 0, 0, 0, 0, 0]

            #Initialize color sensing (scan twice and take average)
            command = 0x01
            for i in range(0,8):
                self.bus.write_byte(self.addr, command)
                data = self.bus.read_i2c_block_data(0x29, 0)
                self.cleari[i] = data[1] << 8 | data[0]  
                self.redi[i] = (data[3] << 8 | data[2])*10000/self.cleari[i]  
                self.greeni[i] = (data[5] << 8 | data[4])*10000/self.cleari[i]
                self.bluei[i] = (data[7] << 8 | data[6])*10000/self.cleari[i]
                command = command << 1
                i = i + 1
                time.sleep(.01)

            command = 0x01
            for i in range(0,8):
                self.bus.write_byte(self.addr, command)
                data = self.bus.read_i2c_block_data(0x29, 0)
                self.cleari[i] = ((data[1] << 8 | data[0])+self.cleari[i])/2  
                self.redi[i] = (((data[3] << 8 | data[2])*10000/self.cleari[i])+self.redi[i])/2    
                self.greeni[i] = (((data[5] << 8 | data[4])*10000/self.cleari[i])+self.greeni[i])/2
                self.bluei[i] = (((data[7] << 8 | data[6])*10000/self.cleari[i])+self.bluei[i])/2
                command = command << 1
                i = i + 1
                time.sleep(.01)

        except IOError:
            print "\nERROR: Color sensors disconnected! Continuing anyway."
        except ZeroDivisionError:
            print "ERROR: Divide by zero. Rescanning..."
            self.initializeColorSensors()

    # Scan blocks for color
    def scanBlocks(self):
        try:
            # Scan blocks
            command = 0x01
            time.sleep(0.01)
            for i in range(0,8):
                time.sleep(.01)
                self.bus.write_byte(self.addr, command)
                command = command << 1

                data = self.bus.read_i2c_block_data(0x29, 0)
                self.clear[i] = data[1] << 8 | data[0]
                self.red[i] = (data[3] << 8 | data[2])*10000/self.clear[i]    
                self.green[i] = (data[5] << 8 | data[4])*10000/self.clear[i]
                self.blue[i] = (data[7] << 8 | data[6])*10000/self.clear[i]
                
                if(i == 0):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 500):
                            if((self.blue[i]-self.bluei[i] >= 20) and (self.blue[i]-self.bluei[i] <= 30) or (self.blue[i]-self.bluei[i] > 50)):
                                self.blockColors[i] = 1
##                            elif((self.green[i] - self.greeni[i] > 10)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
                            elif((self.green[i]-self.greeni[i] > 125)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1000):
                            if((self.red[i]-self.redi[i] > 210)):
                                self.blockColors[i] = 3
                            elif((self.green[i]-self.greeni[i] > -50)):
                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 0
                        #Upper
                        else:
                            if((self.red[i]-self.redi[i] > 1000)):
                                self.blockColors[i] = 3
                            elif((self.green[i]-self.greeni[i] > -500)):
                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 0
                    else:
                        self.blockColors[i] = 0

                if(i == 1):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 100):
                            if((self.blue[i]-self.bluei[i] > 50)):
                                self.blockColors[i] = 1
                            elif((self.green[i] - self.greeni[i] > 10)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
                            elif((self.green[i]-self.greeni[i] > 125)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 10):
                        #Lower
                        if(self.clear[i]-self.cleari[i] < 800):
                            if(self.clear[i]-self.cleari[i] > 200):
                                self.blockColors[i] = 4
##                        if (self.clear[i]-self.cleari[i] < 1000):
##                            if((self.red[i]-self.redi[i] > 210)):
##                                self.blockColors[i] = 3
##                            elif((self.green[i]-self.greeni[i] > -50)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                        #Upper
                        else:
                            if((self.red[i]-self.redi[i] > 1000)):
                                self.blockColors[i] = 3
                            elif((self.green[i]-self.greeni[i] > -500)):
                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 0
                    else:
                        self.blockColors[i] = 0

                if(i == 2):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 100):
                            if((self.blue[i]-self.bluei[i] > 50)):
                                self.blockColors[i] = 1
                            elif((self.green[i] - self.greeni[i] > 10)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
                            elif((self.green[i]-self.greeni[i] > 125)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1000):
                            if((self.red[i]-self.redi[i] > 210)):
                                self.blockColors[i] = 3
                            elif((self.green[i]-self.greeni[i] > -50)):
                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 0
                        #Upper
                        else:
                            if((self.red[i]-self.redi[i] > 1000)):
                                self.blockColors[i] = 3
                            elif((self.green[i]-self.greeni[i] > -500)):
                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 0
                    else:
                        self.blockColors[i] = 0

                if(i == 3):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 100):
                            if((self.blue[i]-self.bluei[i] > 50)):
                                self.blockColors[i] = 1
                            elif((self.green[i] - self.greeni[i] > 10)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
                            elif((self.green[i]-self.greeni[i] > 125)):
                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 0
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1000):
                            if (self.clear[i]-self.cleari[i] > 200):
                                self.blockColors[i] = 4
##                            if((self.red[i]-self.redi[i] > 210)):
##                                self.blockColors[i] = 3
##                            elif((self.green[i]-self.greeni[i] > -50)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                        #Upper
                        else:
                            if((self.red[i]-self.redi[i] > 1000)):
                                self.blockColors[i] = 3
                            elif((self.green[i]-self.greeni[i] > -500)):
                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 0
                    else:
                        self.blockColors[i] = 0

                if(i == 4):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -7):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 900):
                            if((self.blue[i]-self.bluei[i] > 80)):
                                self.blockColors[i] = 1
##                            elif((self.green[i] - self.greeni[i] > 10)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
##                            elif((self.green[i]-self.greeni[i] > 125)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 0):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1000):
                            if (self.clear[i]-self.cleari[i] > 300):
                                self.blockColors[i] = 4
##                            if((self.red[i]-self.redi[i] > 210)):
##                                self.blockColors[i] = 3
##                            elif((self.green[i]-self.greeni[i] > -50)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                        #Upper
                        else:
                            if (self.clear[i]-self.cleari[i] > 8000):
                                self.blockColors[i] = 4
##                            elif((self.green[i]-self.greeni[i] > -500)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                    else:
                        self.blockColors[i] = 0

                if(i == 5):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -5):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 800):
                            if((self.blue[i]-self.bluei[i] > 80)):
                                self.blockColors[i] = 1
##                            elif((self.green[i] - self.greeni[i] > 10)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
##                            elif((self.green[i]-self.greeni[i] > 125)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 0):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1000):
                            if (self.clear[i]-self.cleari[i] > 300):
                                self.blockColors[i] = 4
##                            elif((self.green[i]-self.greeni[i] > -50)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                        #Upper
                        else:
                            if (self.clear[i]-self.cleari[i] > 6000):
                                self.blockColors[i] = 4
##                            elif((self.green[i]-self.greeni[i] > -500)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                    else:
                        self.blockColors[i] = 0

                if(i == 6):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -5):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 800):
                            if((self.blue[i]-self.bluei[i] > 80)):
                                self.blockColors[i] = 1
##                            elif((self.green[i] - self.greeni[i] > 10)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
##                            elif((self.green[i]-self.greeni[i] > 125)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 10):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1000):
                            if (self.clear[i]-self.cleari[i] > 300):
                                self.blockColors[i] = 4
##                            elif((self.green[i]-self.greeni[i] > -50)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                        #Upper
                        else:
                            if((self.clear[i]-self.cleari[i] > 5000)):
                                self.blockColors[i] = 4
##                            elif((self.green[i]-self.greeni[i] > -500)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                    else:
                        self.blockColors[i] = 0

                if(i == 7):
                    #Blue/Green
                    if(self.red[i]-self.redi[i] < -5):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 600):
                            if((self.blue[i]-self.bluei[i] > 80)):
                                  self.blockColors[i] = 1
##                            if((self.blue[i]-self.bluei[i] > 50)):
##                                self.blockColors[i] = 1
##                            elif((self.green[i] - self.greeni[i] > 10)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                        #Upper
                        else:
                            if((self.blue[i]-self.bluei[i] > 200)):
                                self.blockColors[i] = 1
##                            elif((self.green[i]-self.greeni[i] > 125)):
##                                self.blockColors[i] = 2
                            else:
                                self.blockColors[i] = 2
                    #Red/Yellow
                    elif(self.red[i]-self.redi[i] > 0):
                        #Lower
                        if (self.clear[i]-self.cleari[i] < 1500):
                            if (self.clear[i]-self.cleari[i] > 300):
                                self.blockColors[i] = 4
##                            if((self.red[i]-self.redi[i] > 210)):
##                                self.blockColors[i] = 3
##                            elif((self.green[i]-self.greeni[i] > -50)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                        #Upper
                        else:
                            if(self.clear[i]-self.cleari[i] > 5000):
                                self.blockColors[i] = 4
##                            elif((self.green[i]-self.greeni[i] > -500)):
##                                self.blockColors[i] = 4
                            else:
                                self.blockColors[i] = 3
                    else:
                        self.blockColors[i] = 0

            self.sortedBlockColors[0] = self.blockColors[7]
            self.sortedBlockColors[1] = self.blockColors[1]
            self.sortedBlockColors[2] = self.blockColors[6]
            self.sortedBlockColors[3] = self.blockColors[0]
            self.sortedBlockColors[4] = self.blockColors[5]
            self.sortedBlockColors[5] = self.blockColors[2]
            self.sortedBlockColors[6] = self.blockColors[4]
            self.sortedBlockColors[7] = self.blockColors[3]

            return self.sortedBlockColors

        except IOError:
            print "ERROR: Color sensors disconnected!!!"
            return self.blockColors
        except ZeroDivisionError:
            print "ERROR: Divide by zero. Rescanning..."
            self.scanBlocks()