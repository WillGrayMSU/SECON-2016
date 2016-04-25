#!/usr/bin/env python
# .. -*- coding: utf-8 -*-
#
# ****************************************************************************
# bertha_autonomy.py - Autonomous node for Bertha
# ****************************************************************************
#
# Library Imports
# ---------------
import time
import os, sys
import signal
import pigpio

# ROS Imports
# -----------
import rospy
from std_msgs.msg import Byte
from std_msgs.msg import Bool
from std_msgs.msg import Int16

# Local Imports
# -------------
from bertha_bot_controller import RobotController

# Used to send commands to "visual_processor" node
processVidTopic = rospy.Publisher("std_msgs/Int16", Int16)

# Gui Controller
class Autonomy(object):
    def __init__(self):
        
        # Provides access to:
        # 1) Color sensor data, 2) SPI functions, 3) bump switch
        self.controller = RobotController()
    
        # ROS Subscribers
        rospy.Subscriber("std_msgs/Byte", Byte, self._receiveCartArray)
        rospy.Subscriber("std_msgs/Bool", Bool, self._alignedWithBlocks)

        # Initialize robot in stopped position
        self.controller.SetNavCommand(46)

        # Initialize color sensors
        self.controller.scanBlocks()

        # Rail Cart Attributes
        self.cartArray = [None] * 4    # Rail cart order array
        self.cartNum = 0               # Current car
        self.count = 0                 # Tracks cart # in ROS topic transfer
        self.state = 1                 # The current state of the robot
        self.previousState = 1         # The previous state of the robot
        self.scanComplete = False      # "True" if the rail cart scan is done
        self.allCartsInFrame = False   # "True" if frame contains all railcarts
        self.blocksAligned = False     # "True" if robot is aligned with blocks
        self.alignedWithCart = False   # "True" if 
        self.collectionCount = 0       # Keep track of number of collections
        self.top = True                
        self.colorSensorArray = [0,0,0,0,0,0,0,0]
        self.colorSensorArray_Front = [0,0,0,0]
        self.colorSensorArray_Back = [0,0,0,0]

    #__________________________________________________________________________
    #
    # Full Course State Machine
    #__________________________________________________________________________
    def runCourse(self, firstState = 1):
    
        self.state = firstState 
        while(self.state != -1):

            # --------------------------------------
            # Drive forward until first bump occurs
            # --------------------------------------
            if self.state == 1:
                print "Driving through tunnel"
                self.state = self.bumpState(76, 25, 2)

            # ----------------------------------------
            # Prepare to scan QR codes
            # ----------------------------------------
            elif self.state == 2:
                print "Preparing to scan QR codes"
                # Backup away frtom platform
                self.controller.SetNavCommand(108)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.25)

                # Raise from 5 to 10 inches
                self.controller.SetNavCommand(50)
                time.sleep(5.0)
                self.controller.SetNavCommand(54)
                time.sleep(8.0)

                self.previousState = 2
                self.state = 3

            # ---------------------------------------------
            # Begin scanning for QR codes and moving left
            # ---------------------------------------------
            elif self.state == 3:
                print "Scanning for QR codes"
                # Start scanning for QR codes from right to left
                self.processVideo(2)

                # Begin moving left
                self.controller.SetNavCommand(136)
                self.state = 4


            # -----------------------------
            # Checking alignment
            # -----------------------------
            elif self.state == 4:

                if (self.blocksAligned == True):
                    print "Blocks detected"
                    self.controller.SetNavCommand(46)
                    time.sleep(0.25)
                    self.blocksAligned = False
                    self.state = 5

                elif self.blocksAligned == False:
                    self.state = 4

            
            # ----------------------------------------
            # Raise gate and prepare to collect blocks
            # ----------------------------------------
            elif self.state == 5:

                print "Raising gate"
                # Raise gate
                self.controller.SetSortCommand(5)
                time.sleep(0.5)
                self.controller.SetSortCommand(5)
                time.sleep(4.0)

                self.state = self.bumpState(72, 10, 6)


            # ----------------------------------------
            # Collect blocks
            # ----------------------------------------
            elif self.state == 6:

                print "Collecting blocks"
                self.controller.SetSortCommand(39)
                time.sleep(0.5)
                self.controller.SetSortCommand(39)
                time.sleep(10.0)
                # Increment collection counter
                self.collectionCount += 1
                # Blue blocks only. Color sensors not needed.
                if self.collectionCount == 2:
                    self.colorSensorArray = [1,1,1,1,1,1,1,1]
                    self.state = 21
                elif self.collectionCount == 3:
                    # Backup from platform
                    self.controller.SetNavCommand(108)
                    time.sleep(0.5)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.25)
                    # Rotate left 180 degrees
                    self.controller.SetNavCommand(236)
                    time.sleep(2.9)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.25)

                    self.state = 25

                else:
                    # Backup from platform
                    self.controller.SetNavCommand(108)
                    time.sleep(0.5)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.25)
                    # Lower to 5 inches
                    self.controller.SetNavCommand(58)
                    time.sleep(5.0)
                    self.controller.SetNavCommand(62)
                    time.sleep(8.0)
                    # Rotate left 90 degrees
                    self.controller.SetNavCommand(236)
                    time.sleep(1.5)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.25)
                    # Move right into wall
                    self.controller.SetNavCommand(172)
                    time.sleep(1.5)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.25)
                    # Move left off wall
                    self.controller.SetNavCommand(140)
                    time.sleep(0.25)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)
                    
                    self.state = 7


            # ---------------------------------------------
            # Appraoch first rail cart
            # ---------------------------------------------
            elif self.state == 7:

                print "Positioning for rail cart scan"

                self.cartNum = 0

                self.state = self.bumpState(76, 5.0, 8)

            # ----------------------------------------
            # Backup from cart
            # ----------------------------------------  
            elif self.state == 8:
                self.controller.SetNavCommand(108)
                time.sleep(1.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                self.controller.SetNavCommand(172)
                time.sleep(1.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                self.state = 9

            # ----------------------------------------
            # Scan individual rail car
            # ----------------------------------------
            elif self.state == 9:
                print "Scanning rail carts"
                # Scan rail car
                time.sleep(2.0)
                self.processVideo(7)
            
                self.state = 10

            # ---------------------------------------
            # Create cart array when scan is complete
            # ---------------------------------------
            elif self.state == 10:
            
                if self.scanComplete == False:
                    # Continue with scan
                    pass 

                elif self.scanComplete == True:
                    self.cartNum += 1
                    print "Rail car array: ", self.cartArray
                    self.scanComplete = False
                    self.state = 11

            # ----------------------------------------
            # Backup from cart
            # ----------------------------------------  
            elif self.state == 11:

                if self.cartNum != 4:
                    # Move left
                    self.controller.SetNavCommand(140)
                    time.sleep(2.0)
                    self.controller.SetNavCommand(46)
                    time.sleep(2.0)

                    self.state = 9
                else:
                    self.controller.SetNavCommand(108)
                    time.sleep(0.5)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)
                    # Rotate left 90 degrees
                    self.controller.SetNavCommand(204)
                    time.sleep(1.5)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)
                    self.state = self.bumpState(76, 7.0, 12)
                        

            # ---------------------------------------
            # Prepare to align with first cart
            # ---------------------------------------
            elif self.state == 12:

                # Backup from platform
                self.controller.SetNavCommand(108)
                time.sleep(0.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Rotate left 90 degrees
                self.controller.SetNavCommand(236)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Move right into wall
                self.controller.SetNavCommand(172)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Move left off wall
                self.controller.SetNavCommand(140)
                time.sleep(0.25)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                self.controller.SetNavCommand(50)
                # Raise from 5 to 10 inches
                self.controller.SetNavCommand(50)
                time.sleep(5.0)
                self.controller.SetNavCommand(54)
                time.sleep(8.0)
    
                self.state = self.bumpState(72, 5.0, 14)
                
                # Pause For Manual Loading
                print "Scanning blocks onboard"
                self.colorSensorArray = self.controller.scanBlocks()
                #time.sleep(20.0)
                time.sleep(1.0)
                self.colorSensorArray = self.controller.scanBlocks()
                self.top = True
                

            # ---------------------------------------
            # Create Color Sensor Array
            # ---------------------------------------
            elif self.state == 14:
                
                print "All:  ",self.colorSensorArray

                # Create front array and back array
                for sensor in xrange(len(self.colorSensorArray)):
                    if (sensor % 2) == 0:
                        self.colorSensorArray_Front[sensor/2] = self.colorSensorArray[sensor]
                    elif (sensor % 2) == 1:
                        self.colorSensorArray_Back[(sensor-1)/2] = self.colorSensorArray[sensor]

                print "Front:",self.colorSensorArray_Front
                print "Back: ",self.colorSensorArray_Back

                self.cartNum = 0  
                self.state = 15


            # ---------------------------------------
            # First Cart position 1
            # ---------------------------------------
            elif self.state == 15:
                print "First Position", self.cartNum + 1
                #Check channel 3
                if (self.colorSensorArray_Front[2] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[2] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Front[2] != 5):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject FULL block
                    time.sleep(3.0)   
                    self.blockEject(2,True,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[2], " in channel 3"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    self.controller.SetSortCommand(6)
                    time.sleep(3.0)    
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[2] = 5
                    self.colorSensorArray_Back[2] = 5

                elif (self.colorSensorArray_Front[2] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[2] != self.cartArray[self.cartNum]):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject HALF block
                    time.sleep(3.0)   
                    self.blockEject(2,False,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[2], " in channel 3"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    self.controller.SetSortCommand(6)
                    time.sleep(3.0)  
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[2] = self.colorSensorArray_Back[2]
                    self.colorSensorArray_Back[2] = 5

                #Check channel 4
                if (self.colorSensorArray_Front[3] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[3] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Front[3] != 5):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject FULL block
                    time.sleep(3.0)   
                    self.blockEject(3,True,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[3], " in channel 4"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    self.controller.SetSortCommand(6)
                    time.sleep(3.0)  
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[3] = 5
                    self.colorSensorArray_Back[3] = 5

                elif (self.colorSensorArray_Front[3] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[3] != self.cartArray[self.cartNum]):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject HALF block
                    time.sleep(3.0)   
                    self.blockEject(3,False,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[3], " in channel 4"
                    time.sleep(1.5)
                    self.controller.SetSortCommand(6)
                    time.sleep(0.5)    
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[3] = self.colorSensorArray_Back[3]
                    self.colorSensorArray_Back[3] = 5 

                # Backup from first rail cart
                self.controller.SetNavCommand(108)
                time.sleep(0.75)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Adjust on wall after first cart
                if self.cartNum == 0:
                    self.controller.SetNavCommand(172)
                    time.sleep(1.0)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)

                    # Move left
                    self.controller.SetNavCommand(140)
                    time.sleep(1.12)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)

                else:
                    # Move left
                    self.controller.SetNavCommand(140)
                    time.sleep(1.15)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)

                self.controller.SetNavCommand(72)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                self.controller.SetNavCommand(204)
                time.sleep(0.15)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                if (self.checkBlockArray()):
                    self.state = 16
                else:
                    self.controller.SetNavCommand(108)
                    time.sleep(1.0)
                    self.state = 17

            # ---------------------------------------
            # First cart position 2
            # ---------------------------------------
            elif self.state == 16:
                # SECOND POSITION
                print "SECOND POSITION CART", self.cartNum + 1
                #Check channel 1
                if (self.colorSensorArray_Front[0] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[0] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Front[0] != 5):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject FULL block
                    time.sleep(3.0)   
                    self.blockEject(0,True,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[0], " in channel 1"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(6)
                    time.sleep(3.0)   
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[0] = 5
                    self.colorSensorArray_Back[0] = 5

                elif (self.colorSensorArray_Front[0] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[0] != self.cartArray[self.cartNum]):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject HALF block
                    time.sleep(3.0)   
                    self.blockEject(0,False,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[0], " in channel 1"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(6) 
                    time.sleep(3.0)   
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[0] = self.colorSensorArray_Back[0]
                    self.colorSensorArray_Back[0] = 5

                #Check channel 2
                if (self.colorSensorArray_Front[1] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[1] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Front[1] != 5):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject FULL block
                    time.sleep(3.0)   
                    self.blockEject(1,True,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[1], " in channel 2"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(6)  
                    time.sleep(3.0)
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[1] = 5
                    self.colorSensorArray_Back[1] = 5

                elif (self.colorSensorArray_Front[1] == self.cartArray[self.cartNum]) and (self.colorSensorArray_Back[1] != self.cartArray[self.cartNum]):
                    self.controller.SetSortCommand(5)
                    #Raise Gate
                    self.controller.SetSortCommand(5)
                    # Eject HALF block
                    time.sleep(3.0)   
                    self.blockEject(1,False,self.top)
                    print "Ejecting ", self.colorSensorArray_Front[1], " in channel 2"
                    time.sleep(1.5)  
                    self.controller.SetSortCommand(6)
                    time.sleep(0.5)  
                    self.controller.SetSortCommand(6)  
                    time.sleep(3.0)
                    # Insert ejected value into color sensor array
                    self.colorSensorArray_Front[1] = self.colorSensorArray_Back[1]
                    self.colorSensorArray_Back[1] = 5

                self.cartNum += 1

                # Close 
                self.controller.SetSortCommand(4)
                time.sleep(1.0)

                if (self.checkBlockArray()):
                    if (self.cartNum == 4):
                        self.controller.SetNavCommand(108)
                        time.sleep(1.0)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)

                        # END
                        self.state = 17

                    else:
                        # Back up
                        self.controller.SetNavCommand(108)
                        time.sleep(0.75)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)
                        # Move left
                        self.controller.SetNavCommand(140)
                        time.sleep(0.7)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)
                        # Run sonar
                        self.controller.SetNavCommand(72)
                        time.sleep(1.5)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)
                        self.state = 15
                        
                        #self.state = self.bumpState(76, 15)
                else:
                    self.state = 17
                    self.controller.SetNavCommand(108)
                    time.sleep(1.0)
                    self.controller.SetNavCommand(46)
                    time.sleep(0.5)
                    
            # ------------------------------------
            # Move back to beginning of rail carts
            # ------------------------------------
            elif self.state == 17:
                # Rotate right 90 degrees
                self.controller.SetNavCommand(204)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                self.state = self.bumpState(76, 20, 18)
                
            # -----------------------------------
            # Face first rail cart
            # -----------------------------------
            elif self.state == 18:
                # Back up from wall
                self.controller.SetNavCommand(108)
                time.sleep(0.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Rotate left 90 degrees
                self.controller.SetNavCommand(236)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Move right into wall
                self.controller.SetNavCommand(172)
                time.sleep(2.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                self.controller.SetNavCommand(136)
                time.sleep(0.5)
                # Stop
                self.controller.SetNavCommand(46)
                time.sleep(1.0)

                self.state = self.bumpState(76, 3, 19)

            # --------------------------------
            # Check blocks onboard
            # --------------------------------
            elif self.state == 19:
                if self.top:
                    if (self.checkBlockArray()):
                        #self.state = self.bumpState(76, 15)
                        self.state = 15
                        self.cartNum = 0
                    else:
                        if (self.checkForError()):
                            self.controller.SetSortCommand(5)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(5)
                            time.sleep(1.0)
                            self.controller.SetSortCommand(15)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(15)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(17)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(17)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(19)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(19)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(21)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(21)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(6)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(6)
                            time.sleep(1.0)
                        #If there are no blocks left on top, then retract top pistons, scan blocks and bump into first cart.
                        self.controller.SetSortCommand(22)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(22)
                        time.sleep(2.0)
                        self.controller.SetSortCommand(20)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(20)
                        time.sleep(2.0)
                        self.controller.SetSortCommand(18)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(18)
                        time.sleep(2.0)
                        self.controller.SetSortCommand(16)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(16)
                        time.sleep(2.0)
                        #Set Top to false, reset cartNum, scan bottom blocks
                        self.top = False
                        self.cartNum = 0
                        self.colorSensorArray = self.controller.scanBlocks()
                        self.state = 14

                else:
                    if (self.checkBlockArray()):
                        #self.state = self.bumpState(76, 15)
                        self.state = 15
                        #self.runSonar(self.thresh, 72, 15)
                        self.cartNum = 0
                    else:
                        if (self.checkForError()):
                            self.controller.SetSortCommand(5)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(5)
                            time.sleep(1.0)
                            self.controller.SetSortCommand(7)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(7)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(9)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(9)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(11)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(11)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(13)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(13)
                            time.sleep(2.0)
                            self.controller.SetSortCommand(6)
                            time.sleep(0.5)
                            self.controller.SetSortCommand(6)
                            time.sleep(0.5)
                        # Retract bottom pistons
                        self.controller.SetSortCommand(14)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(14)
                        time.sleep(2.0)
                        self.controller.SetSortCommand(12)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(12)
                        time.sleep(2.0)
                        self.controller.SetSortCommand(10)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(10)
                        time.sleep(2.0)
                        self.controller.SetSortCommand(8)
                        time.sleep(0.5)
                        self.controller.SetSortCommand(8)
                        time.sleep(2.0)

                        # Back-up 
                        self.controller.SetNavCommand(108)
                        time.sleep(1.0)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)
                        # Move left
                        self.controller.SetNavCommand(140)
                        time.sleep(1.0)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)
                        # Rotate right
                        self.controller.SetNavCommand(204)
                        time.sleep(1.5)
                        self.controller.SetNavCommand(46)
                        time.sleep(0.5)

                        # Enter bump state
                        self.state = self.bumpState(76, 5, 20)
                

            # ---------------------------------------------
            # Prepare to scan QR codes on 7 inch platform
            # ---------------------------------------------
            elif self.state == 20:
                print "Preparing to scan QR codes"
                # Backup away frtom platform
                self.controller.SetNavCommand(108)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.55)

                # Lower from 10 to 7 inches
                self.controller.SetNavCommand(62)
                time.sleep(8.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                self.state = 3

            # Deliver blocks to boat
            elif self.state == 21:
                print "Preparing to deliver to boat"
                # Backup from platform
                self.controller.SetNavCommand(108)
                time.sleep(1.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Move right slightly
                self.controller.SetNavCommand(172)
                time.sleep(0.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Rotate left 180 degrees
                self.controller.SetNavCommand(204)
                time.sleep(2.90)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Drive towards boat
                self.controller.SetNavCommand(76)
                time.sleep(5.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                print "Depositing blocks onto boat"
                # Raise gate
                self.controller.SetSortCommand(5)
                time.sleep(0.5)
                self.controller.SetSortCommand(5)
                time.sleep(4.0)
                # Eject all blocks
                self.controller.SetSortCommand(1)
                time.sleep(0.5)
                self.controller.SetSortCommand(1)
                time.sleep(3.0)
		        # Lower gate
                self.controller.SetSortCommand(6)
                time.sleep(0.5)
                self.controller.SetSortCommand(6)
                time.sleep(4.0)
                # Retract all
                self.controller.SetSortCommand(2)
                time.sleep(0.5)
                self.controller.SetSortCommand(2)
                time.sleep(3.0)

                self.state = 22
            # --------------------------------
            # Leave boat
            # --------------------------------
            elif self.state == 22:
                print "Leaving boat"
                # Backup from boat
                self.controller.SetNavCommand(108)
                time.sleep(3.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Rotate left 90 degrees
                self.controller.SetNavCommand(236)
                time.sleep(1.4)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Enter bump state
                self.state = self.bumpState(76, 10.0, 23)

            # ---------------------------------------------
            # Prepare to collect from load 5 inch platform
            # ---------------------------------------------
            elif self.state == 23:
                print "Preparting to collect blocks"
                # Backup from wall
                self.controller.SetNavCommand(108)
                time.sleep(0.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Rotate left 90 degrees
                self.controller.SetNavCommand(236)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Move right into wall
                self.controller.SetNavCommand(172)
                time.sleep(1.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Move left off wall
                self.controller.SetNavCommand(140)
                time.sleep(0.15)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)
                # Lower from 7 to 5 inches
                self.controller.SetNavCommand(58)
                time.sleep(5.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Enter bump state
                self.state = self.bumpState(72, 7.0, 24)

            # --------------------------------------------
            # Prepare to scan QR codes on 5 inch platform
            # --------------------------------------------
            elif self.state == 24:
                # Backup away from platform
                self.controller.SetNavCommand(108)
                time.sleep(1.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.25)
                # Move right into wall
                self.controller.SetNavCommand(172)
                time.sleep(1.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.25)

                self.state = 3
            # --------------------------------
            # Deliver blocks to truck
            # --------------------------------
            elif self.state == 25:
                # Move right
                self.controller.SetNavCommand(172)
                time.sleep(2.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move forward
                self.controller.SetNavCommand(76)
                time.sleep(4.25)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move left to align with tunnel
                self.controller.SetNavCommand(140)
                time.sleep(3.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move right
                self.controller.SetNavCommand(168)
                time.sleep(0.25)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Raise gate
                self.controller.SetSortCommand(5)
                time.sleep(0.5)
                self.controller.SetSortCommand(5)
                time.sleep(4.0)

                # Raise from 5 to 7 inches
                self.controller.SetNavCommand(50)
                time.sleep(5.0)

                # Move forward
                self.controller.SetNavCommand(76)
                time.sleep(2.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move backward
                self.controller.SetNavCommand(108)
                time.sleep(0.15)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move left
                self.controller.SetNavCommand(140)
                time.sleep(0.25)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move right
                self.controller.SetNavCommand(172)
                time.sleep(0.25)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Eject Channels 1 and 2 Bottom
                self.controller.SetSortCommand(13)
                self.controller.SetSortCommand(13)
                time.sleep(3.0)
                self.controller.SetSortCommand(11)
                self.controller.SetSortCommand(11)
                time.sleep(3.0)
                # Retract Channels 1 and 2 Bottom
                self.controller.SetSortCommand(14)
                self.controller.SetSortCommand(14)
                time.sleep(3.0)
                self.controller.SetSortCommand(12)
                self.controller.SetSortCommand(12)
                time.sleep(3.0)
                # Eject Channels 1 and 2 Bottom
                self.controller.SetSortCommand(13)
                self.controller.SetSortCommand(13)
                time.sleep(3.0)
                self.controller.SetSortCommand(11)
                self.controller.SetSortCommand(11)
                time.sleep(3.0)
                # Retract Channels 1 and 2 Bottom
                self.controller.SetSortCommand(14)
                self.controller.SetSortCommand(14)
                time.sleep(3.0)
                self.controller.SetSortCommand(12)
                self.controller.SetSortCommand(12)
                time.sleep(3.0)

                # Backup away from truck
                self.controller.SetNavCommand(180)
                time.sleep(0.15)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Move right
                self.controller.SetNavCommand(172)
                time.sleep(0.5)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Eject Channels 3 and 4 Bottom
                self.controller.SetSortCommand(7)
                time.sleep(3.0)
                self.controller.SetSortCommand(9)
                time.sleep(3.0)
                # Retract Channels 3 and 4 Bottom
                self.controller.SetSortCommand(8)
                time.sleep(3.0)
                self.controller.SetSortCommand(10)
                time.sleep(3.0)
                # Eject Channels 3 and 4 Bottom
                self.controller.SetSortCommand(7)
                time.sleep(3.0)
                self.controller.SetSortCommand(9)
                time.sleep(3.0)
                # Retract Channels 3 and 4 Bottom
                self.controller.SetSortCommand(8)
                time.sleep(3.0)
                self.controller.SetSortCommand(10)
                time.sleep(3.0)

                # Backup away from truck
                self.controller.SetNavCommand(108)
                time.sleep(1.0)
                self.controller.SetNavCommand(46)
                time.sleep(0.5)

                # Lower gate
                self.controller.SetSortCommand(6)
                time.sleep(0.5)
                self.controller.SetSortCommand(6)
                time.sleep(4.0)

                # Enter end state
                self.state = 100

            # -----------------------------
            # End state
            # -----------------------------
            elif self.state == 100:
                print "End"
                self.controller.SetNavCommand(46)   # Stop movement
                self.controller.SetSortCommand(40)  # Clear sort command
                time.sleep(0.5)

                self.state = -1

            time.sleep(0.1)
                
        sys.exit(0)
        


    def blockEject(self, channel, full, top):
        channel +=1
        print channel
        if top:
            if full:
                if channel == 1:
                    self.controller.SetSortCommand(21)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(21)
                    time.sleep(0.5)
                elif channel == 2:
                    self.controller.SetSortCommand(19)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(19)
                    time.sleep(0.5)
                elif channel == 3:
                    self.controller.SetSortCommand(17)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(17)
                    time.sleep(0.5)
                elif channel == 4:
                    self.controller.SetSortCommand(15)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(15)
                    time.sleep(0.5)
            else:
                if channel == 1:
                    self.controller.SetSortCommand(37)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(37)
                    time.sleep(0.5)
                elif channel == 2:
                    self.controller.SetSortCommand(35)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(35)
                    time.sleep(0.5)
                elif channel == 3:
                    self.controller.SetSortCommand(33)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(33)
                    time.sleep(0.5)
                elif channel == 4:
                    self.controller.SetSortCommand(31)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(31)
                    time.sleep(0.5)
        else:
            if full:
                if channel == 1:
                    self.controller.SetSortCommand(13)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(13)
                    time.sleep(0.5)
                elif channel == 2:
                    self.controller.SetSortCommand(11)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(11)
                    time.sleep(0.5)
                elif channel == 3:
                    self.controller.SetSortCommand(9)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(9)
                    time.sleep(0.5)
                elif channel == 4:
                    self.controller.SetSortCommand(7)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(7)
                    time.sleep(0.5)

            else:
                if channel == 1:
                    self.controller.SetSortCommand(29)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(29)
                    time.sleep(0.5)
                elif channel == 2:
                    self.controller.SetSortCommand(27)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(27)
                    time.sleep(0.5)
                elif channel == 3:
                    self.controller.SetSortCommand(25)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(25)
                    time.sleep(0.5)
                elif channel == 4:
                    self.controller.SetSortCommand(23)
                    time.sleep(0.5)
                    self.controller.SetSortCommand(23)
                    time.sleep(0.5)
        time.sleep(0.5)

    def checkBlockArray(self):
        blocksLeft = False
        for block in range(0,4):
            if self.colorSensorArray_Front[block] != 5:
                blocksLeft = True
        return blocksLeft

    def checkForError(self):
        print self.colorSensorArray
        error = False
        for block in range(0,4):
            if self.colorSensorArray_Front[block] == 0:
                error = True
            if self.colorSensorArray_Back[block] == 0:
                error = True
        return error

    def bumpState(self, speed, timeout, nextState):
        self.controller.bumpSwitchPressed = False
        self.controller.SetNavCommand(speed)
        timer = time.time()
        elapsed = time.time()
        print "Waiting for bump"
        while (self.controller.bumpSwitchPressed == False) and (timeout > (elapsed-timer)):
            # Move forward until bump switch is pressed
            elapsed = time.time()

        if timeout > (elapsed-timer):
            print "Timeout"

        time.sleep(0.1)
        self.controller.bumpSwitchPressed = False
        time.sleep(0.1)
        self.controller.SetNavCommand(46)
        time.sleep(0.5)

        return nextState


    # Sends command to visual_processor
    # ---------------------------------
    # command = 1: Scan for QR codes left to right
    # command = 2: Scan rail carts and create cart array
    # command = 3: Align with rail cart
    def processVideo(self, command):
        processVidTopic.publish(command)


    # ROS Subscriber handler
    # ----------------------
    # aligned = "True" when aligned with blocks
    def _alignedWithBlocks(self, aligned):

        if (aligned.data == True) and (self.state == 4):
            self.blocksAligned = True
        elif (aligned.data == True) and (self.state == 10):
            self.alignedWithCart = True
        elif (aligned.data == True) and (self.state == 12):
            self.alignedWithCart = True 

    # ROS Subscriber handler
    # ----------------------
    # Returns each element of cart array individually. "Count" keeps track of the position in the array.
    def _receiveCartArray(self, index):

        self.scanComplete = True
        #print index.data

        if(self.count == 0):
            self.createCartArray(0, index.data)
        elif(self.count == 1):
            self.createCartArray(1, index.data)
        elif(self.count == 2):
            self.createCartArray(2, index.data)
        elif(self.count == 3):
            self.createCartArray(3, index.data)
        elif(self.count == 4):
            self.count = 0
        
        self.count += 1

    def createCartArray(self, cartNum, cartColor):
        self.cartArray[cartNum] = cartColor

    #Reverse railcart array for second course orientation.
    def reverseArray(self, array):
        newArray = [None] * len(array)
        for index in xrange(len(array)):
            newArray[index] = array[len(array)-(index+1)]
        return newArray

def stop(signal, frame):
        stopController = RobotController()
        stopController.SetNavCommand(46)
        sys.exit(0)
    
# Setup the application
def main(autoNode=Autonomy):

    #Register Stop Command Signal Handler
    signal.signal(signal.SIGINT,stop)

    # Initialize "Bertha_Autonomy" ROS node
    rospy.init_node('Bertha_Autonomy', anonymous=True)
    
    # Create Autonomy object
    node1 = autoNode()

    # Start state machine at initial state
    node1.runCourse(1)

    # Assures that ROS topics stay alive until script ends
    rospy.spin()

if __name__=='__main__':
    main()