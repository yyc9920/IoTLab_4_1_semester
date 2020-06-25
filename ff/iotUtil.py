import uuid
import time

def get_uuid():
    return str(uuid.uuid4())

def newEendpoint(id, nick, desc, vendor):
    return {
        "endpointId": id,
        "friendlyName": nick,
        "description": desc,
        "manufacturerName": vendor,
        "capabilities": [
		    {
		        "type": "AlexaInterface",
		        "interface": "Alexa.EndpointHealth",
		        "version": "3",
		        "properties": {
		            "supported":[
		                { "name":"connectivity" }
		            ],
		            "proactivelyReported": True,
		            "retrievable": True
		        }
		    },
		    {
		        "type": "AlexaInterface",
		        "interface": "Alexa",
		        "version": "3"
		    }
        ]
    }

def addCap(iface, names):
    m = {
        "type": "AlexaInterface",
        "interface": iface,
        "version": "3",
        "properties": {
            "supported": [],
            "proactivelyReported": True,
            "retrievable": True
        }
    }
    for n in names:
        m["properties"]["supported"].append({ "name": n })
    return m

def get_utc_timestamp(seconds=None):
    return time.strftime("%Y-%m-%dT%H:%M:%S.00Z", time.gmtime(seconds))
    
def buildResponse(ctxspace, ctxname, value, evtspace, evtname, request):
    return {
        "context": {
            "properties": [
                {
                    "namespace": ctxspace,
                    "name": ctxname,
                    "value": value,
                    "timeOfSample": get_utc_timestamp(),
                    "uncertaintyInMilliseconds": 1000
                }
            ]
        },
        "event": {
            "header": {
                "namespace": evtspace,
                "name": evtname,
                "payloadVersion": "3",
                "messageId": get_uuid(),
                "correlationToken": request["directive"]["header"]["correlationToken"]
            },
            "endpoint": {
                "scope": {
                    "type": "BearerToken",
                    "token": "access-token-from-Amazon"
                },
                "endpointId": request["directive"]["endpoint"]["endpointId"]
            },
            "payload": {}
        }
    }
