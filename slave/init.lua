--WiFi SetUp
station_cfg={}
station_cfg.ssid="WLAN_SSID"
station_cfg.pwd="WLAN_PWD"
wifi.sta.config(station_cfg)

--Mqtt SetUp
mqttKeepAlive = 60; --sec
host = "bro.ker.add.res"
port = "1883"


m = mqtt.Client("backlighter", mqttKeepAlive);
mqttMainTopic = "/desk/backlight"

--functions
function split(s, sep)
    local fields = {}

    local sep = sep or " "
    local pattern = string.format("([^%s]+)", sep)
    string.gsub(s, pattern, function(c) fields[#fields + 1] = c end)

    return fields
end

function ws2812NewSettings(configs)
    file.open(configfile, "w+")
    file.writeline("LEDs="..configs[1])
    file.writeline("Colours="..configs[2])
    file.close()
end


--WS2812 SetUp
configfile = ".config"
if not file.exists(configfile) then
    BytesPerLed = 4
    NumOfLeds = 1
    ws2812NewSettings({NumOfLeds,BytesPerLed})
else
    file.open(configfile, "r")
    line = file.readline()
    while line ~= nil do
        line = split(line,"=")
        if line[1] == "LEDs" then
            NumOfLeds = line[2]
        elseif line[1] == "Colours" then
            BytesPerLed = line[2]
        end
        line = file.readline()
    end
    file.close()
end

bfrleft = ws2812.newBuffer(NumOfLeds, BytesPerLed)
--bfrright = ws2812.newBuffer(NumOfLeds, BytesPerLed)
--bfrright:fill(0,0,0,0)

bfrleft:fill(0,0,0,0)
bfrleft:set(1,0,0,0,255)
ws2812.init(ws2812.MODE_SINGLE)
ws2812.write(bfrleft)
bfrleft:set(1,0,0,0,0)

--for led=1,8,1 --64 is included
--do 
--    bfrleft:set(led,2^(led-1),0,0,0)
--end
--for led=9,16,1 --64 is included
--do 
--    bfrleft:set(led,0,2^(led-9),0,0)
--end
--for led=17,24,1 --64 is included
--do 
--    bfrleft:set(led,0,0,2^(led-17),0)
--end
--for led=25,32,1 --64 is included
--do 
--    bfrleft:set(led,0,0,0,2^(led-25))initializing
--end
--for led=33,40,1 --64 is included
--do 
--    bfrleft:set(led,0,0,2^(led-32),2^(led-32))
--end

tmr.alarm(1,1000, 1, function()
    if wifi.sta.getip()==nil then
    else
        tmr.stop(1)
        --host, port, secure, autoreconnect,function on success, function on fail
    m:connect(host, port,0 ,0,
    function (client)
        m:subscribe({[mqttMainTopic .. "/numcolours"]=0, --number of colours, each LED has (RGB, RGBW, RGBWW)
                     [mqttMainTopic .. "/numleds"]=0,   --number of LEDs, which are connected via WS2812
                     [mqttMainTopic .. "/colour"]=0},   --byte-stream, which contains the colour-settings (incl. white)
                                                        --two cases:
                                                          --multiple of NumColours -> LEDs starting at 1st, will be set
                                                          --multiple of NumColours + 1 -> first byte will be interpreted as Offset of LEDs (so max is 255)
            function(conn)
--                print("subscribe success")
                bfrleft:set(1,255,0,0,0)
                ws2812.write(bfrleft)
            end)
    end,
    function(client,reason)
--        print("connection failed: "..reason)
        bfrleft:set(1,0,255,0,0)
        ws2812.write(bfrleft)
    end)

    end
end)

--interpreting of incoming messages
m:on("message", function(client, topic, data)
    if (topic == mqttMainTopic .. "/numcolours") and
       (string.len(data) == 1) and
       (tonumber(data) >=3 and tonumber(data) <=5) then
        BytesPerLed = data;
        ws2812NewSettings({NumOfLeds,data})
    elseif (topic == mqttMainTopic .. "/numleds") then
        NumOfLeds = data;
        ws2812NewSettings({NumOfLeds,BytesPerLed})
    elseif topic == mqttMainTopic .. "/colour" then
        if (string.len(data) % BytesPerLed) == 0 then
            bfrleft:replace(data)
        elseif (string.len(data) % BytesPerLed) == 1 then
            bfrleft:replace(string.sub(data,1),string.byte(data,1))
        end
        ws2812.write(bfrleft)
    end
end)
