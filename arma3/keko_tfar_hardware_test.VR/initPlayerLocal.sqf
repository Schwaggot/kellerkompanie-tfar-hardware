// code from http://killzonekid.com/arma-scripting-tutorials-how-to-make-arma-extension-part-4/

keko_tfar_hardware_fnc_callExtensionAsync = {
    [format [
        "r:%1",
        "keko_tfar_hardware" callExtension format [
            "s:%1", 
            _this select 0
        ]
    ], _this select 1] spawn {
        waitUntil {
            _res = "keko_tfar_hardware" callExtension (_this select 0);
            if (_res != "WAIT") exitWith {
                _res call (_this select 1);
                true
            };
            false
        };
    };
};

keko_tfar_hardware_fnc_callBackFunc = {  
    //systemChat format ["_this: %1", _this];  

    if(count _this != 0) then {   
        parseSimpleArray _this params ["_channel","_additionalChannel","_volume","_frequencies"];
        //systemChat format ["current channel: %1, additional channel: %2, volume: %3", _channel, _additionalChannel, _volume];
        //systemChat format ["frequencies: %1", _frequencies];
     

        private _activeSWRadio = call TFAR_fnc_activeSwRadio;
        //[_activeSWRadio, _frequencies] call TFAR_fnc_setSwSettings;
        [_activeSWRadio, _channel - 1] call TFAR_fnc_setSwChannel;
        [_activeSWRadio, _volume] call TFAR_fnc_setSwVolume;

        if(_additionalChannel > 0) then {
            [_activeSWRadio, _additionalChannel - 1] call TFAR_fnc_setAdditionalSwChannel;
        };   

        {
            if(_x >= 30 && _x <= 512) then {
                [_activeSWRadio, _forEachIndex + 1, str(_x)] call TFAR_fnc_SetChannelFrequency;
            };            
        } forEach _frequencies;
    
    }
    else {
        systemChat "keko_tfar_hardware: ERROR";
    };
      
};

keko_tfar_hardware_fnc_callBackFuncInfo = {
    systemChat format ["return: %1", _this];       
};

player addAction [ "Update from hardware", { _id = ["get", keko_tfar_hardware_fnc_callBackFunc] call keko_tfar_hardware_fnc_callExtensionAsync; } ];
player addAction [ "Disconnect from hardware", { _id = ["disconnect", keko_tfar_hardware_fnc_callBackFuncInfo] call keko_tfar_hardware_fnc_callExtensionAsync; } ];
player addAction [ "Connect to hardware", { _id = ["connect", keko_tfar_hardware_fnc_callBackFuncInfo] call keko_tfar_hardware_fnc_callExtensionAsync; } ];
player addAction [ "Start loop", { 
    _handle = [] spawn {
        //_id = ["connect", keko_tfar_hardware_fnc_callBackFuncInfo] call keko_tfar_hardware_fnc_callExtensionAsync;
        keko_tfar_hardware_loop = true;
        publicVariable "keko_tfar_hardware_loop";
 
        while {keko_tfar_hardware_loop} do {
            if !(player call TFAR_fnc_isSpeaking) then {
                _id = ["get", keko_tfar_hardware_fnc_callBackFunc] call keko_tfar_hardware_fnc_callExtensionAsync; 
            };
            systemChat "keko_tfar_hardware: UPDATE";
            sleep 5;
        };
    };

} ];
player addAction [ "Stop loop", { 
        keko_tfar_hardware_loop = false;
        publicVariable "keko_tfar_hardware_loop";    
} ];


