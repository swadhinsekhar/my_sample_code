# gstreamer_commands
Gstreamer commands
```
## h264encoder video filsrc to udpsink

$ gst-launch-1.0 -ev filesrc location=file.mp4 ! qtdemux ! h264parse ! rtph264pay config-interval=1 timestamp=0 seqnum=0 perfect-rtptime=true ! udpsink clients=192.168.0.60:5000,192.168.0.60:5001 auto-multicast=true sync=true

## udpsrc to xvimagesink using h264decoder

$ gst-launch-1.0 -v udpsrc port=5001 caps="application/x-rtp, clock-rate=90000,encoding-name=H264,payload=96" ! queue ! rtph264depay ! h264parse ! avdec_h264 ! xvimagesink sync=false

## filesrc to tcpserversink, h264decoder to theoraenc

$ gst-launch-1.0 -ev filesrc location=file.mp4 ! qtdemux ! h264parse ! avdec_h264 lowres=0 ! capsfilter caps="video/x-raw, format=(string)I420, width=(int)640, height=(int)360" ! videoconvert ! queue ! theoraenc ! oggmux ! tcpserversink host=192.168.0.60 port=8099

## webcam video streaming over udpsink using h264encoder

$ gst-launch-1.0 -v v4l2src device=/dev/video0 do-timestamp=true ! "video/x-raw, format=I420, width=640, height=480, framerate=30/1" ! queue ! x264enc ! h264parse ! rtph264pay config-interval=1 ! udpsink host=192.168.0.60 port=5001


gst-launch-1.0 videotestsrc is-live=true ! x264enc ! mpegtsmux ! hlssink

gst-launch-1.0 videotestsrc is-live=true ! queue ! x264enc ! mpegtsmux ! udpsink host=239.1.10.100 port=1234 auto-multicast=true

gst-launch-1.0 -v filesrc location=audio-video.pcap ! pcapparse src-port=1 ! "application/x-rtp, payload=111, encoding-name=OPUS" ! rtpopusdepay ! fakesink
```
