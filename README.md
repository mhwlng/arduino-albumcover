# arduino-albumcover

Experiment to show album cover from home assistant media player on M5Stack dial and core2 displays (ESP32)

![dial](https://i.imgur.com/suuqg7B.jpg)

1. This project requires https://github.com/imgproxy/imgproxy to resize the image from home asssistant and convert from .png to .jpg.

   To create an imgproxy docker container, listening on port 8088:

   ```
   sudo docker run -d \
     --name=imgproxy \
     -p 8088:8080  \
     --restart unless-stopped \
     darthsim/imgproxy
   ```
  
2. This project requires an MQTT broker (like the Mosquitto broker add-on in Home Assistant) 

3. This project requires Home Assistant, to create an MQTT topic, for the media player entity_picture.

   One way to do that, is to use the mqtt_statestream integration:

   https://www.home-assistant.io/integrations/mqtt_statestream/

   ```
   mqtt_statestream:
     base_topic: hass
     publish_attributes: true
     publish_timestamps: true
     exclude:
       entity_globs:
       - device_tracker.*
       - ......
   ```

   ![mqtt](https://i.imgur.com/W8y48h6.png)
   
   

The 8.8 inch touch screen, in above picture, is connected to a raspberry pi and can show Home Assistant dashboards.
More information can be found here:

https://github.com/mhwlng/kiosk-server

The 3d-printed enclosure can be found here:

https://www.printables.com/@mhwlng_888536/collections/920676



note that espMqttClient.h references this specific library:

https://github.com/bertmelis/espMqttClient
 