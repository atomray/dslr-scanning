# Additional Notes

### Camera Infrared

Once I discovered that the camera had an IR receiver I started doing some research - 
I had no idea how IR signalling worked in general. Interesting to discover that they
do high frequency pulses to differentiate the signal from background noise.

Fortunately I was able to dig up some documentation / reverse engineering of the communication
protocol for the Olympus E-510. I'll like mirror some of it here for posterity, during my
research I hit quite a few dead links.

Technical details of the RM-1 remote:
[How to make your own Olympus RM-1 compatible remote](https://adapting-camera.blogspot.com/2018/08/how-to-make-your-own-olympus-rm-1.html)

Source code that I pulled from (didn't want to try using libraries in Arduino just yet):
[MajicDesigns / MD_multiCameraIrControl](https://github.com/MajicDesigns/MD_multiCameraIrControl)




