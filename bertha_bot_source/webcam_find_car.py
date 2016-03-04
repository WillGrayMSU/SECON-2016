# .. -*- coding: utf-8 -*-
#
# *********************************************************************
# webcam_find_car.py - Image processing and UI for color blob detection
# *********************************************************************
# This module provides support code for driving a remote-controlled car based on feedback from a webcam. At its heart, this programs calls a user-supplied ``update`` function with a webcam image and provides routines to send commands to the car. It consists of the following functionality:
#
# - A main loop to grab an image from the webcam then call ``update``.
# - Image-processing code, :func:`find_car`, which determines the X, Y center of a given color (typically the color of the R/C car).
#
# .. _WebcamFindCar:
#
# Main loop
# =========
# The following class supplies code to grab an image from the webcam then call ``update``. In addition, it:
#
# - Creates a trackbar to allow easy of the :attr:`threshold` used by :func:`find_car`.
# - Handles mouse clicks by recording the color of the clicked pixel to use with :func:`find_car`.
#
# To do this, we rely on libraries which do most of the work. In particular, the following line provides access to the powerful image-processing routines provided by `OpenCV <docs.opencv.org>`_.
import cv2
# OpenCV_ relies heavily on `NumPy <numpy.scipy.org>`_ to manipulate arrays containing image data.
import numpy as np
from math import sqrt, pi, sin, cos, atan2, copysign
from numpy import polyfit


# Round f then convert it to an f. f can be a scalar or a tuple.
def round_int(f):
    if isinstance(f, tuple):
        return tuple(int(round(x)) for x in f)
    return int(round(f))

# This function finds a color blob (assumed to be the car), outlining it and returning its center.
def find_car(image, lab_color, thresh):
    lab_image = image / np.float32(255.0)
    contours = find_lab_color(lab_image, lab_color, thresh)
    cont_image, mass_center, cont_area = draw_car_contour(image, contours)
    return lab_image, cont_image, mass_center, cont_area

# This routine takes an image in the Lab color space, a color to find in that image, and a threshold around that color, then returns contours surrounding this color.
def find_lab_color(lab_image, color, thresh):
    assert(color.dtype == np.float32)
# Compute (image - target_color)^2, giving a Euclidian distance between the two.
    diff_image = lab_image - color
    normsq_image = np.sum(diff_image*diff_image, -1)
# `cv2.Threshold <http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html#threshold>`_ the image to select only pixels close to the target color. Convert it from floating-point back to an 8-bit image, since the steps below require 8-bit input.
    (retval, thresh_image) = cv2.threshold(normsq_image, thresh**2.0, 255.0, cv2.THRESH_BINARY_INV)
    thresh_image = np.uint8(thresh_image)
# Perform a morphological open (`erode <http://docs.opencv.org/modules/imgproc/doc/filtering.html#cv2.erode>`_ then dilate), using `getStructuringElement <http://docs.opencv.org/modules/imgproc/doc/filtering.html#getstructuringelement>`_.
    sel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    it = 2
    erode_image = cv2.erode(thresh_image, sel, iterations=it)
    open_image = cv2.dilate(erode_image, sel, iterations=it)
# Find the contours of the image, smooth them, and draw them
    contours0, hierarchy = cv2.findContours(open_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#    contours = [cv2.approxPolyDP(cnt, 3, True) for cnt in contours0]
    contours = contours0
    return contours

# Given a contour, outline it and find its center.
def draw_car_contour(image, contours):
    if not contours:
        return image, (-1, -1), 0
# Select the largest-area contour, then compute its mass center
    cont_area = [cv2.contourArea(contour) for contour in contours]
    max_cont_area_index = np.argmax(cont_area)
    mass_center = compute_mass_center(contours[max_cont_area_index])
#    print(cont_area, max_cont_area_index, mass_center)
    cont_image = image.copy()
    cv2.drawContours(cont_image, contours, max_cont_area_index, (0, 0, 255), 3)
    # http://docs.opencv.org/modules/core/doc/drawing_functions.html#cv2.circle
    cv2.circle(cont_image, round_int(mass_center), 10, (0, 255, 255), -1)
    return cont_image, mass_center, cont_area[max_cont_area_index],

# Found this formula at    http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html#moments
def compute_mass_center(contour):
    moments = cv2.moments(contour)
    m00 = moments['m00']
    if m00 == 0:
        return -1, -1
    m01 = moments['m01']
    m10 = moments['m10']
    x_m = m10/m00
    y_m = m01/m00
    return x_m, y_m


