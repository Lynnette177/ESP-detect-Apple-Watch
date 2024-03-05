#include <NimBLEDevice.h>

void scanEndedCB(NimBLEScanResults results);

static NimBLEAdvertisedDevice* advDevice;

static bool doConnect = false;
static uint32_t scanTime = 10; /** 0 = scan forever */

bool applewatchhere = false;
unsigned int lastspot = 0;
bool turn_on_by_ble = false;

void scanEndedCB(NimBLEScanResults results){
    Serial.println("Scan Ended");
}

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
        Serial.println("Connected");
        pClient->updateConnParams(120,120,0,60);
    };
    void onDisconnect(NimBLEClient* pClient) {
        Serial.print(pClient->getPeerAddress().toString().c_str());
        Serial.println(" Disconnected - Starting scan");
        NimBLEDevice::getScan()->start(scanTime, scanEndedCB);
    };
    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) {
        if(params->itvl_min < 24) { /** 1.25ms units */
            return false;
        } else if(params->itvl_max > 40) { /** 1.25ms units */
            return false;
        } else if(params->latency > 2) { /** Number of intervals allowed to skip */
            return false;
        } else if(params->supervision_timeout > 100) { /** 10ms units */
            return false;
        }

        return true;
    };

    uint32_t onPassKeyRequest(){
        Serial.println("Client Passkey Request");
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key){
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        return true;
    };
    void onAuthenticationComplete(ble_gap_conn_desc* desc){
        if(!desc->sec_state.encrypted) {
            Serial.println("Encrypt connection failed - disconnecting");
            NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
            return;
        }
    };
};
class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {

    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (!advertisedDevice->haveManufacturerData()) return;
        std::string strmanu = advertisedDevice->getManufacturerData();
        int i=0;
        int appledevice = 0;
        for(char c : strmanu){
          if(i==0 && c==0x4c){appledevice++;}
          if(i==1 && c==0){appledevice++;}
          if(i==2 && c==0x10){appledevice++;}
          if(i==3 && c==0x05){appledevice++;}
          i++;
        }
        if(appledevice == 4)
        {
          int rssi = advertisedDevice->getRSSI();
          Serial.print(advertisedDevice->getRSSI());
          if(rssi > -64){
            applewatchhere = true;
            lastspot = millis();
            Serial.println("AppleWatch Spooted!!!");
            NimBLEDevice::getScan()->stop();
            advDevice = advertisedDevice;
          }
        }
    };
};
