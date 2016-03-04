#!/usr/bin/env python
# .. -*- coding: utf-8 -*-
#
# *********************************************************************
# bertha_bot_base - Code for controlling the SECON 2016 robot, Bertha.
# *********************************************************************
# This provide a framework for controlling the bot, by
# displaying and opening a simple GUI.
#
# Imports
# =======
# Library imports
import sys
from os.path import dirname, join

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
from sensor_msgs.msg import Image    	 # for receiving the video feed
from sensor_msgs.msg import CompressedImage

# Numpy Imports
import numpy as np

# Local imports
from webcam_find_car import find_car
from bertha_bot_controller import RobotController

# John's import
#from PIL import Image

# Gui Controller
class ButtonGui(QDialog):
    def __init__(self):
        # Always do Qt init first.
        QDialog.__init__(self)

        self.controller = RobotController()

        # Set up the user interface from Designer.
        uic.loadUi(join(dirname(__file__), 'bertha_bot_control.ui'), self)
        self.setWindowTitle('SECON 2016 Video Feed')
        self.cv = CvBridge()

        self.trackingColor = np.array([1, 0, 0], dtype=np.float32)
        self.state = 1
        self.x_center = -1
        self.y_center = -1

        # Initialize GUI labels
        self.lbAuto_2.setText('[ 0. 0. 0.]')
        self.updateCenterPositions('0', '0')

        self.state = 1

        # Rail cart stuff
        self.count = 0
        self.railCartArray = [0, 0, 0, 0]

        self.firstCart_x_Position = [20, 60]
        self.secondCart_x_Position = [100, 140]
        self.thirdCart_x_Position = [180, 220]
        self.fourthCart_x_Position = [250, 290]


        self.redCartColor = [60, 8, 0]
        self.blueCartColor = [1, 19, 19]
        self.greenCartColor = [2, 19, 0]
        self.yellowCartColor = [96, 89, 0]
        self.railCartColorMatrix = [self.redCartColor, self.blueCartColor, self.greenCartColor, self.yellowCartColor]


    def videoFrame(self, image):

        #self.cv_image = self.cv.imgmsg_to_cv2(image, "rgb8")
        self.cv_image = np.asarray(self.cv.imgmsg_to_cv(image, "rgb8"))
        self.cv_image = cv2.resize(self.cv_image, (self.cv_image.shape[1]/2, self.cv_image.shape[0]/2))

        # lab_img, cont_image, center_mass, cont_area = find_car(self.cv_image, self.trackingColor, self.hsThreshold.value()/100.0)
        lab_img, cont_image, center_mass, cont_area = find_car(self.cv_image, self.trackingColor, 0.05)

        qi_1 = QImage(cont_image.data, cont_image.shape[1], cont_image.shape[0], QImage.Format_RGB888)
        
        # Video Feed
        self.lbVideo.setFixedHeight(cont_image.shape[0])
        self.lbVideo.setFixedWidth(cont_image.shape[1])
        self.lbVideo.setPixmap(QPixmap.fromImage(qi_1))

        # Center coordinates of tracked blob
        self.x_center = center_mass[0]
        self.y_center = center_mass[1]

        self.updateCenterPositions(str(self.x_center),str(self.y_center))

        # Enable autonomous mode
        if self.cbAuto.isChecked():
            self.updateState(self.state)
        else:
            self.lbAuto.setText('Disabled.')

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
            self.updateTrackingColorLabel(str(self.trackingColor))
            print self.cv_image[y,x]

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
