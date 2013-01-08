kinecamok
=========

Recieves face tracking data from FaceTrackingUdpSender.
The Kinect space is mapped similar the way geometry is mapped in mapamok.
Press 'n' to use the tip of the nose as a mapping point, or 'e' to use a point between the eyebrows.
Press the middle mouse button to switch modes. Move the point onto either the nose of the tracked face,
or between the eyebrows.
Press the middle mouse again to confirm. And again to set a next point. 
Move the tracked face around the whole range of view while this process.

Once the calibration is done switch to face shading mode and view the videos on your face...

Installation
=========

Install OpenFrameworks.
Install ProCamToolkit from https://github.com/YCAMInterlab/ProCamToolkit/
Place this next to mapamok.
copy the data dir from mapamok/bin/ to kinecamok/bin/ and add the required movies.
