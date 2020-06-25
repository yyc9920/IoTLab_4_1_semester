from signal import pause
import gpiozero
r = gpiozero.OutputDevice(22, active_high=True, initial_value=False)
button = gpiozero.Button(21)
button.when_pressed = r.on
button.when_released = r.off
pause() 
