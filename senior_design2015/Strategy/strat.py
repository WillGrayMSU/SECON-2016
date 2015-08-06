# /*********************************************************************
# *
# * Mississippi State University
# *
# *********************************************************************
# * FileName: strat.py
# *
# *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# * MODULE FUNCTION: Basically operates as a timer for the course then
# * adds up all the points the robot gianed.
# *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# * Author                Date                    Comment
# *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# * Steven Calhoun        9/26/2014               SECON 2015
# *********************************************************************/


import time
import math

ROBOT_NAME = "Bonnie"

MAX_TIME =          300
MAX_POINTS =        590

START_POINTS =      100
TO_GAME_POINTS =    10
SIMON_POINTS =      150
ETCH_POINTS =       200
RUBIKS_POINTS =     50
CARD_POINTS =       50

elapsedTime = 0
totalFollowTime = 0
totalPoints = 0


def waitForStart():
    global totalPoints

    print("Waiting for start signal")
    print (raw_input('Hit enter when ' + ROBOT_NAME + ' starts'))
    print("Starting")

    totalPoints += START_POINTS
    return


def followToGame():
    global totalPoints

    startFollowTime = time.time()
    print("Following to game")
    print (raw_input('Hit enter when ' + ROBOT_NAME + ' arrives at the next game'))

    totalPoints += TO_GAME_POINTS
    return time.time() - startFollowTime


def playSimon():
    global totalPoints

    startSimonTime = time.time()
    print("Playing Simon")
    print (raw_input('Hit enter when ' + ROBOT_NAME + ' finishes Simon'))

    totalPoints += SIMON_POINTS
    return time.time() - startSimonTime


def playEtchASketch():
    global totalPoints

    startEtchTime = time.time()
    print("Playing Etch-a-Sketch")
    print (raw_input('Hit enter when ' + ROBOT_NAME + ' finishes with the Etch'))

    totalPoints += ETCH_POINTS
    return time.time() - startEtchTime


def playRubik():
    global totalPoints

    startRubikTime = time.time()
    print("Playing Rubik's Cube")
    print (raw_input('Hit enter when ' + ROBOT_NAME + ' twists the cube'))

    totalPoints += RUBIKS_POINTS
    return time.time() - startRubikTime


def playCards():
    global totalPoints

    startCardsTime = time.time()
    print("Playing Cards")
    print (raw_input('Hit enter when ' + ROBOT_NAME + ' twists the cube'))

    totalPoints += CARD_POINTS
    return time.time() - startCardsTime


def goToFinish():
    global totalPoints

    startTime = time.time()
    print(raw_input('Hit enter when ' + ROBOT_NAME + ' crosses the finish line'))

    return time.time() - startTime


if __name__ == "__main__":
    # Wait for start
    waitForStart()

    startTime = time.time()

    # Follow line to game
    totalFollowTime += followToGame()

    # Play Simon
    simonTime = playSimon()

    # Follow line to game
    totalFollowTime += followToGame()

    # Play Rubik's Cube
    rubikTime = playRubik()

    # Follow line to game
    totalFollowTime += followToGame()

    # Play Etch-a-Sketch
    etchTime = playEtchASketch()

    # Follow line to game
    totalFollowTime += followToGame()

    # Play Cards
    cardTime = playCards()

    # Follow line to finish
    goToFinishTime = goToFinish()

    # Time points
    elapsedTime = totalFollowTime + goToFinishTime + simonTime + etchTime + rubikTime + cardTime

    print("Simon time: " + str(simonTime))
    print("Rubiks time: " + str(rubikTime))
    print("Etch time: " + str(etchTime))
    print("Card time: " + str(cardTime))
    print("Total run time: " + str(elapsedTime) + "/" + str(MAX_TIME))
    print("Total points: " + str(math.trunc(totalPoints)) + "/" + str(MAX_POINTS))
