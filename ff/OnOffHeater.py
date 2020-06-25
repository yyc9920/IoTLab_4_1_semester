import requests
import logging
import json
from iotUtil import *

logger = logging.getLogger()
logger.setLevel(logging.INFO)

header_on = {'valve': 'on', 'content-type': 'application/json'}
header_off = {'valve': 'off', 'content-type': 'application/json'}
iotWeb='https://your.nodered.com:1880/thermostat'

def lambda_handler(request, context):
    logger.info(json.dumps(request, indent=4, sort_keys=True))
    if request['directive']['header']['name'] == 'Discover':
        response = handleDiscovery(request, context)
    else:
        response = handleControl(request, context)
    logger.info(json.dumps(response, indent=4, sort_keys=True))
    return response

def handleDiscovery(request, context):
    thermo = buildThermo()
    response = {
        "event": {
            "header": {
                "namespace": "Alexa.Discovery",
                "name": "Discover.Response",
                "payloadVersion": "3",
                "messageId": get_uuid()
            },
            "payload": {
                "endpoints": [ thermo ]
            }
        }
    }
    return response

def buildThermo():
    ep = newEendpoint("valve", "Thermostat", "Heater Valve", "yhur")
    ep["displayCategories"] = ["OTHER"]
    ep["cookie"] = {}
    ep["capabilities"].append(addCap("Alexa.PowerController",["powerState"]))
    ep["capabilities"].append(addCap("Alexa.ThermostatController",["targetSetpoint", "thermostatMode"]))
    ep["capabilities"].append(addCap("Alexa.TemperatureSensor",["temperature"]))
    return ep

def handleControl(request, context):
    request_namespace = request["directive"]["header"]["namespace"]
    request_name = request["directive"]["header"]["name"]

    if request_namespace == "Alexa.PowerController":
        if request_name == "TurnOn":
            value = "ON"
            result = requests.get(iotWeb, header_on, verify=False)

        else:
            value = "OFF"
            result = requests.get(iotWeb, header_off, verify=False)
        response = buildResponse("Alexa.PowerController", "powerState", value, "Alexa", "Response", request)
        return response

    # other handlers omitted in this example
