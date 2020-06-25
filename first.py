import RPi.GPIO as pi
import time

pi.setmode(pi.BCM)
while(1):
    pi.setup(22, pi.OUT)
    pi.output(22,pi.HIGH)
    time.sleep(3)
    pi.output(22,pi.LOW)
    time.sleep(3)
