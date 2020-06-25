import requests
import logging
import json
from iotUtil import *

logger = logging.getLogger()
logger.setLevel(logging.INFO)

header_on = {'valve': 'on', 'content-type': 'application/json'}
header_off = {'valve': 'off', 'content-type': 'application/json'}
header_level = {'value': value, 'content-type': 'application/json'}
header_leveldata = {'value': value, 'content-type': 'application/json'}
iotWeb='http://54.157.37.148:1880/thermostat'
iotWeb2='http://54.157.37.148:1880/thermostat_level'
iotWeb3='http://54.157.37.148:1880/thermostat_leveldata'

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
	ep["capabilities"].append(addCap("Alexa.PowerLevelController",["powerLevel"]))
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

	if request_namespace == "Alexa.PowerLevelController":
		if request_name == "SetPowerLevel":
			value = request["directive"]["payload"]["powerLevel"]
			result = requests get(iotWeb2, header_level, verify=False)

		else
			value = request["directive"]["payload"]["powerLevelDelta"]
			result = requests get(iotWeb3, header_leveldata, verify=False)

    # other handlers omitted in this example
