# arduino-albumcover

Experiment to show album cover from home assistant media player on arduino display (ESP32)

This Requires https://github.com/imgproxy/imgproxy to resize the image and convert from .png to .jpg.

To crearte a docker container listening on port 8088:

`sudo docker run -d \
  --name=imgproxy \
  -p 8088:8080\
  --restart unless-stopped \
  darthsim/imgproxy`
  
  