import time
import os
import pigpio

import rospy
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
from std_msgs.msg import Byte
from std_msgs.msg import String

spi = spidev.SpiDev()

directionThresh = 70

def navigation(data):
    
    if (abs(data.linear.x) < 80) and (abs(data.linear.y) < 80) and (abs(data.angular.z) < 80):
        print "STOPPED..."
        command = 0x01
	spi.open(0,0)

    elif (abs(data.linear.x) < directionThresh) and (data.linear.y == 80) and (abs(data.angular.z) < directionThresh):
        print "MOVING FORWARD..."
        command = 0x02
	spi.open(0,0)

    elif (abs(data.linear.x) < directionThresh) and (data.linear.y == -80) and (abs(data.angular.z) < directionThresh):
        command = 0x03
        print "MOVING BACKWARD..."
	spi.open(0,0)	

    elif (data.linear.x == 80) and (abs(data.linear.y) < directionThresh) and (abs(data.angular.z) < directionThresh):
        command = 0x04
        print "MOVING LEFT..."
	spi.open(0,0)	

    elif (data.linear.x == -80) and (abs(data.linear.y) < directionThresh) and (abs(data.angular.z) < directionThresh):
        command = 0x05
        print "MOVING RIGHT..."
	spi.open(0,0)

    elif (abs(data.linear.x) < directionThresh) and (abs(data.linear.y) < directionThresh) and (data.angular.z == 80):
        command = 0x06
        print "ROTATING LEFT..."
	spi.open(0,0)

    elif (abs(data.linear.x) < directionThresh) and (abs(data.linear.y) < directionThresh) and (data.angular.z == -80):
        command = 0x07
        print "ROTATING RIGHT..."
	spi.open(0,0)	
  
    else:
        command =  0x01
        spi.open(0,0)
    
    # Send the command over SPI
    resp = spi.xfer2([command])
    spi.close()

    previousCommand = command

def button(data):
    command = 0x17
   
    if(data == Byte(1)):
	command = 0x01
   	print('All Forward')
  	spi.open(0,1)
    elif(data == Byte(2)):
	print("All Backward")
	spi.open(0,1)
	command = 0x02
    elif(data == Byte(3)):
	print("Arm Forward")
	spi.open(0,1)
	command = 0x03
    elif(data == Byte(4)):
	print("Arm Backward")
	spi.open(0,1)
	command = 0x04
    elif(data == Byte(5)):
	print("Gate Up")
	spi.open(0,1)
	command = 0x05
    elif(data == Byte(6)):
	print("Gate Down")
	spi.open(0,1)
	command = 0x06
    elif(data == Byte(7)):
	print("Motor 4 Forward")
	spi.open(0,1)
	command = 0x07
    elif(data == Byte(8)):
	print("Motor 4 Backward")
	spi.open(0,1)
	command = 0x08
    elif(data == Byte(9)):
	print("Motor 3 Forward")
	spi.open(0,1)
	command = 0x09
    elif(data == Byte(10)):
	print("Motor 3 Backward")
	spi.open(0,1)
	command = 0x0A
    elif(data == Byte(11)):
	print("Motor 2 Forward")
	spi.open(0,1)
	command = 0x0B
    elif(data == Byte(12)):
	print("Motor 2 Backward")
	spi.open(0,1)
	command = 0x0C
    elif(data == Byte(13)):
	print("Motor 1 Forward")
	spi.open(0,1)
	command = 0x0D
    elif(data == Byte(14)):
	print("Motor 1 Backward")
	spi.open(0,1)
	command = 0x0E
    elif(data == Byte(15)):
	print("Motor 8 Forward")
	spi.open(0,1)
	command = 0x0F
    elif(data == Byte(16)):
	print("Motor 8 Backward")
	spi.open(0,1)
	command = 0x10
    elif(data == Byte(17)):
	print("Motor 7 Forward")
	spi.open(0,1)
	command = 0x11
    elif(data == Byte(18)):
	print("Motor 7 Backward")
	spi.open(0,1)
	command = 0x12
    elif(data == Byte(19)):
	print("Motor 6 Forward")
	spi.open(0,1)
	command = 0x13
    elif(data == Byte(20)):
	print("Motor 6 Backward")
	spi.open(0,1)
	command = 0x14
    elif (data == Byte(21)):
	print("Motor 5 Forward")
	spi.open(0,1)
	command = 0x15
    elif(data == Byte(22)):
	print("Motor 5 Backward")
	spi.open(0,1)
	command = 0x16
    elif(data == Byte(23)):
	print("Motor 4 Half Forward")
	spi.open(0,1)
	command = 0x17
    elif(data == Byte(24)):
	print("Motor 4 Half Backward")
	spi.open(0,1)
	command = 0x18
    elif(data ==Byte(25)):
	print("Motor 3 Half Forward")
	spi.open(0,1)
	command = 0x19
    elif(data == Byte(26)):
	print("Motor 3 Half Backward")
	spi.open(0,1)
	command = 0x1A
    elif(data == Byte(27)):
	print("Motor 2 Half Forward")
	spi.open(0,1)
	command = 0x1B
    elif(data == Byte(28)):
	print("Motor 2 Half Backward")
	spi.open(0,1)
	command = 0x1C
    elif(data == Byte(29)):
	print("Motor 1 Half Forward")
	spi.open(0,1)
	command = 0x1D
    elif(data == Byte(30)):
	print("Motor 1 Half Backward")
	spi.open(0,1)
	command = 0x1E
    elif(data == Byte(31)):
	print("Motor 8 Half Forward")
	spi.open(0,1)
	command = 0x1F
    elif(data == Byte(32)):
	print("Motor 8 Half Backward")
	spi.open(0,1)
	command = 0x20
    elif(data == Byte(33)):
	print("Motor 7 Half Forward")
	spi.open(0,1)
	command = 0x21
    elif(data == Byte(34)):
	print("Motor 7 Half Backward")
	spi.open(0,1)
	command = 0x22
    elif(data == Byte(35)):
	print("Motor 6 Half Forward")
	spi.open(0,1)
	command = 0x23
    elif(data == Byte(36)):
	print("Motor 6 Half Backward")
	spi.open(0,1)
	command = 0x24
    elif(data == Byte(37)):
	print("MOtor 5 Half Forward")
	spi.open(0,1)
	command = 0x25
    elif(data == Byte(38)):
	print("Motor 5 Half Backward")
	spi.open(0,1)
	command = 0x26
    elif(data == Byte(39)):
	print("All Backward Half")
	spi.open(0,1)
	command = 0x27
    else:
        command = 0x28
        spi.open(0,1)

    resp = spi.xfer2([command])
    spi.close()


def main():
    
    rospy.init_node('SECON_SPI', anonymous=True)
    rospy.Subscriber("geometry_msgs/command_velocity", Twist, navigation)
    rospy.Subscriber("std_msgs/Byte", Byte, button)
  
    rospy.spin()

if __name__ == '__main__':
    main()
