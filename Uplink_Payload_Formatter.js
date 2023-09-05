var RELAY_STATUS = ["off", "on"]
var PAYLOAD_TYPE = ["Temp&Humid","RelayStatus"]

function decodeUplink(input) {
  
  if (input.bytes[0] == 0)
  {
    var temp = (input.bytes[1] << 8 | input.bytes[2])
    var humid = (input.bytes[3] << 8 | input.bytes[4])
    
    return {
      data: {
        payloadType: PAYLOAD_TYPE[input.bytes[0]],
        temperature: temp,
        humidity: humid,
      },
      warnings: [],
      errors: []
    };
  }
  else if (input.bytes[0] == 1)
  {
    return {
      data: {
        payloadType: PAYLOAD_TYPE[input.bytes[0]],
        relayStatus: RELAY_STATUS[input.bytes[1]]
      },
      warnings: [],
      errors: []
    };
  }
}