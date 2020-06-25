import wiotp.sdk
import RPi.GPIO as g
from signal import pause
import time
deviceOptions = {
 "identity": {"orgId": "wla4dm", "typeId": "raspberrypi", "deviceId": "iotDev4"},
  "auth": {"token": "Fgh)9f26nAAxxe(b7Y"},
}
data = {
   "d" : {
   }
}
def commandProcessor(cmd):
    print(cmd.data["d"])
    if cmd.data["d"]["lamp"]:
        if cmd.data["d"]["lamp"] == "on":
            g.output(22, g.HIGH)
            data["d"]["lamp"] = "on"
        else:
            g.output(22, g.LOW)
            data["d"]["lamp"] = "off"
        deviceCli.publishEvent("status", "json", data, qos=0)

g.setmode(g.BCM)
g.setup(22, g.OUT)
g.setup(21,g.IN)
deviceCli = wiotp.sdk.device.DeviceClient(deviceOptions)
deviceCli.commandCallback = commandProcessor
deviceCli.connect()
while(1):
    if (g.input(21)==0):
        data["d"]["switch"] = "clicked"
    else:
        data["d"]["switch"] = "released"
    time.sleep(10)
    deviceCli.publishEvent("status","json",data,qos=0)
