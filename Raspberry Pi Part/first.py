import RPi.GPIO as pi

pi.setmode(pi.BCM)
pi.setup(22, pi.OUT)
pi.output(22,pi.LOW)
