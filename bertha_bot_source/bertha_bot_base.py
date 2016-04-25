#!/usr/bin/env python
# .. -*- coding: utf-8 -*-
#
# *********************************************************************
# bertha_bot_base - Code for controlling the SECON 2016 robot, Bertha.
# *********************************************************************
# This provides a framework for controlling the bot, by
# displaying and opening a simple GUI.
#
# Imports
# =======
# Library imports
import sys
from os.path import dirname, join
import time

# Sip imports
import sip
sip.setapi('QString', 2)
sip.setapi('QVariant', 2)

# OpenCV Imports
import cv2
from cv_bridge import CvBridge  # CvBridgeError

# Qt Imports
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic

# ROS Imports
import rospy
from sensor_msgs.msg import Image
from std_msgs.msg import Byte
from std_msgs.msg import Bool
from std_msgs.msg import Int16

# Numpy Imports
import numpy as np

# Local imports
from webcam_find_car import find_car
from bertha_bot_controller import RobotController


# Gui Controller
class ButtonGui(QDialog):
    def __init__(self):
        # Always do Qt init first.
        QDialog.__init__(self)

        self.controller = RobotController()

        # ROS Publishers
        self.test = rospy.Publisher('std_msgs/Byte', Byte)
        self.alignPub = rospy.Publisher('std_msgs/Bool', Bool)

        # ROS Subscribers
        rospy.Subscriber("std_msgs/Int16", Int16, self._receiveCommand)

        # Set up the user interface from Designer.
        # ----------------------------------------
        uic.loadUi(join(dirname(__file__), 'bertha_bot_control.ui'), self)
        self.setWindowTitle('SECON 2016 Video Feed')
        self.cv = CvBridge()

        # Blob detection attributes
        # -------------------------
        self.trackingColor = np.array([1, 0, 0], dtype=np.float32)
        self.state = 1
        self.x_center = -1
        self.y_center = -1
        self.countRecalulateCenter = 0

        # Initialize GUI labels
        # ---------------------
        self.lbAuto_2.setText('[ 0. 0. 0.]')
        self.updateCenterPositions('0', '0')

        # Rail cart alignment
        # -------------------
        self.scanCarts = False          # "True" if it's time to scan rail carts
        self.thresholdValue = 0.1       # Initial threshold value for blob detection
        self.colorIndex = 0             # Used to keep track of current tracking color   
        self.railCartArray = [0,0,0,0]  # Initial rail cart array values
        self.pixelArray = [None] * 4
        self.x = 160                    # Pixel x location      
        self.y = 120                    # Pixel y location

        # Block alignment
        # ---------------
        self.scanBlocks = False
        self.blocksDetected = False
        self.scanBlocksLeft2Right = False
        self.scanBlocksRight2Left = False
        self.scanAllCarts = False
        self.alignWithCartR2L = False
        self.alignWithCartL2R = False
        self.scanOneCart = False


    # Process each frame from ROS video feed
    def videoFrame(self, image):

        # Convert ROS image to OpenCV image
        self.cv_image = np.asarray(self.cv.imgmsg_to_cv(image, "rgb8"))
        self.cv_image = cv2.resize(self.cv_image, (self.cv_image.shape[1]/2, self.cv_image.shape[0]/2))

        # Crop cv_image
        self.cv_image_cropped = self.cv_image

        # Blob detection algorithm
        lab_img, cont_image, center_mass, cont_area = find_car(self.cv_image_cropped , self.trackingColor, self.thresholdValue)
        #lab_img, cont_image, center_mass, cont_area = find_car(self.cv_image_cropped , self.trackingColor, self.hsThreshold.value()/100.0)
        # Create QImage for GUI video frame
        #qi_1 = QImage(cont_image.data, cont_image.shape[1], cont_image.shape[0], QImage.Format_RGB888)
            
        # Set GUI video feed parameters
        #self.lbVideo.setFixedHeight(cont_image.shape[0])
        #self.lbVideo.setFixedWidth(cont_image.shape[1])
        #self.lbVideo.setPixmap(QPixmap.fromImage(qi_1))

        # Asign the center coordinates of the tracked blob
        self.x_center = center_mass[0]
        self.y_center = center_mass[1]

        # Recalculates the center of the blob
        self.updateCenterPositions(str(self.x_center),str(self.y_center))

        # Scan QR codes from left to right
        if self.scanBlocksLeft2Right == True:
            self.lbAuto.setText('Aligning with blocks.')
            self.blocksDetected = self.blocks_detected2(99,self.cv_image,150,50)
            if self.blocksDetected == True:
                print "Blocks detected"
                self.scanBlocksLeft2Right = False
                self.alignPub.publish(True)
                self.blocksDetected = False

        # Scan QR codes from right to left
        elif self.scanBlocksRight2Left == True:
            self.lbAuto.setText('Aligning with blocks.')
            self.blocksDetected = self.blocks_detected2(210,self.cv_image,150,50)
            if self.blocksDetected == True:
                print "Blocks detected"
                self.scanBlocksRight2Left = False
                self.alignPub.publish(True)
                self.blocksDetected = False

        # Scan rail carts when command is sent from autonomy node
        elif self.scanAllCarts == True:
            self.lbAuto.setText('Scanning rail carts.')
            self.scanRailCarts(self.state)

        # Align with rail cart
        elif self.alignWithCartR2L == True:
            self.lbAuto.setText('Aligning with rail cart.')
            self.thresholdValue = 0.05
            self.alignWithCartRight2Left()

        elif self.alignWithCartL2R == True:
            self.lbAuto.setText('Aligning with rail cart.')
            self.thresholdValue = 0.05
            self.alignWithCartLeft2Right()

        # Scan individual cart color
        elif self.scanOneCart == True:
            self.lbAuto.setText('Scanning cart.')
            self.thresholdValue = 0.1
            colorVal = 0
            # Choose pixel in frame
            colorVal = self.calculateColor(self.cv_image[self.y, self.x])
            self.test.publish(colorVal)
            self.scanOneCart = False


    # Command from autonomy node over ROS topic
    def _receiveCommand(self, command):

        # Scan QR codes from left to right
        if command.data == 1:
            self.scanBlocksLeft2Right = True

        # Scan QR codes from right to left
        elif command.data == 2:
            self.scanBlocksRight2Left = True

        # Scan rail carts
        elif command.data == 3:
            self.scanAllCarts = True

        # Scan rail carts
        #elif command.data == 4:
        #    self.alignWithCart_Right = True

        # Align with cart right to left
        elif command.data == 5:
            self.alignWithCartR2L = True

        # Align with cart left to right
        elif command.data == 6:
            self.alignWithCartL2R = True

        # Determine rail car color
        elif command.data == 7:
            self.scanOneCart = True

    # Determines color based on RGB array
    def calculateColor(self, rgbArray):
        # Blue   = 1
        # Green  = 2
        # Red    = 3
        # Yellow = 4
        print "RGB Array:    ", rgbArray
        print "StdDev Array: ", np.std(rgbArray)
        print "----------------------------------------"
        # Check For Yellow / Red
        if (rgbArray[0] >= 100):
            # Check For Yellow
            if (abs(rgbArray[0]) - abs(rgbArray[1]) <= 20):
                colorVal = 4
            else:
                colorVal = 3
        # Check For Blue
        elif (rgbArray[2] >= 40):
            colorVal = 1
        # Asign Green Value
        else:
            colorVal = 2
        
        return colorVal


    def setTrackingColor(self, rgb_Array):
        self.trackingColor = np.array(rgb_Array, dtype=np.float32)/255.0
        self.updateTrackingColorLabel(str(self.trackingColor))

    # On a mouse press, select a tracking color.
    def mousePressEvent(self, QMouseEvent):
        x = QMouseEvent.x() - self.lbVideo.x()
        y = QMouseEvent.y() - self.lbVideo.y()

        # Only pick a color if the mouse click lies inside the image.
	if x >= 0 and y >= 0 and x < self.lbVideo.width() and y < self.lbVideo.height():
            self.trackingColor = np.array(self.cv_image[y, x], dtype=np.float32)/255.0
            self.updateTrackingColorLabel(str(self.cv_image[y, x]))
            #print self.cv_image[y,x]
            

class RosVideo(QObject):
    videoFrame = pyqtSignal(Image)

    def __init__(self):
        QObject.__init__(self)

    def run(self):
        self.sub = rospy.Subscriber('/camera/image',
          Image, self.videoFrame.emit, queue_size=1)


# Setup the application
def main(gui=ButtonGui):

    # Initialize ROS node
    rospy.init_node("visual_processor", anonymous=True)

    # Create QT object
    app = QApplication(sys.argv)
    window = gui()
    window.show()

    # Create ROS object
    rv = RosVideo()
    rv.videoFrame.connect(window.videoFrame)
    rv.run()

    # Executes the QT application
    status = app.exec_()

    # Stop receiving messages when the windows closes; otherwise,
    # see segfaults.
    rv.sub.unregister()
    sys.exit(status)

if __name__=='__main__':

    main()
