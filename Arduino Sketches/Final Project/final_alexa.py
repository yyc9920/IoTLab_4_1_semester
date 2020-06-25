import requests
import logging
import json
from iotUtil import *

logger = logging.getLogger()
logger.setLevel(logging.INFO)

header_on = {'timer': 'on', 'content-type': 'application/json'}
header_off = {'timer': 'off', 'content-type': 'application/json'}
iotWeb='https://54.157.37.148:1880/add_ing'

def lambda_handler(request, context):
    logger.info(json.dumps(request, indent=4, sort_keys=True))
    if request['directive']['header']['name'] == 'Discover':
        response = handleDiscovery(request, context)
    else:
        response = handleControl(request, context)
        logger.info(json.dumps(response, indent=4, sort_keys=True))
        return response

def handleDiscovery(request, context):
    induction1 = buildInd1()
    induction2 = buildInd2()
    induction3 = buildInd3()
    induction4 = buildInd4()
    timer = buildTimer()
    response = {
        "event": {
            "header": {
                "namespace": "Alexa.Discovery",
                "name": "Discover.Response",
                "payloadVersion": "3",
                "messageId": get_uuid()
            },
            "payload": {
                "endpoints": [ induction1, induction2, induction3, induction4, timer ]
            }
        }
    }
    return response

def buildInd1():
    ep = newEendpoint("valve1", "Induction1", "induction1", "yhur")
    ep["displayCategories"] = ["OTHER1"]
    ep["cookie"] = {}
    ep["capabilities"].append(addCap("Alexa.PowerController",["powerState"]))
    return ep

def buildInd2():
    ep = newEendpoint("valve2", "Induction2", "induction2", "yhur")
    ep["displayCategories"] = ["OTHER2"]
    ep["cookie"] = {}
    ep["capabilities"].append(addCap("Alexa.PowerController",["powerState"]))
    return ep

def buildInd3():
    ep = newEendpoint("valve3", "Induction3", "induction3", "yhur")
    ep["displayCategories"] = ["OTHER3"]
    ep["cookie"] = {}
    ep["capabilities"].append(addCap("Alexa.PowerController",["powerState"]))
    return ep

def buildInd4():
    ep = newEendpoint("valve4", "Induction4", "induction4", "yhur")
    ep["displayCategories"] = ["OTHER4"]
    ep["cookie"] = {}
    ep["capabilities"].append(addCap("Alexa.PowerController",["powerState"]))
    return ep

def buildTimer():
    ep = newEendpoint("cookingtime", "cookingTimer", "cookingtimer", "yhur")
    ep["displayCategories"] = ["OTHERS"]
    ep["cookie"] = {}
    ep["capabilities"].append(addCap("Alexa.PowerController",["powerState"]))
    return ep

def handleControl(request, context):
    request_namespace = request["directive"]["header"]["namespace"]
    request_name = request["directive"]["header"]["name"]

    if request_namespace == "Alexa.PowerController":
        if request_name == "TurnOn":
            value = "ON"

        else:
            value = "OFF"
            if request["directive"]["endpoint"]["endpointId"] == "valve1":
                result = requests.get(iotWeb, {'induction1' : value}, verify=False)
            elif request["directive"]["endpoint"]["endpointId"] == "valve2":
                result = requests.get(iotWeb, {'induction2' : value}, verify=False)
            elif request["directive"]["endpoint"]["endpointId"] == "valve3":
                result = requests.get(iotWeb, {'induction3' : value}, verify=False)
            elif request["directive"]["endpoint"]["endpointId"] == "valve4":
                result = requests.get(iotWeb, {'induction4' : value}, verify=False)
            else:
                result = requests.get(iotWeb, {'timer' : value}, verify=False)
                response = buildResponse("Alexa.PowerController", "powerState", value, "Alexa", "Response", request)
                return response

            # other handlers omitted in this example
