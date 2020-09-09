acoustic-location-system
========================

Software and hardware designs for an acoustic location system suitable for small underwater ROVs such as OpenROV


This project is about developing an acoustic based location system for small ROVs.

Much of the early work that we have been doing is in the blog: https://forum.openrov.com/t/acoustic-modems-location-and-pingers
That blog covers broader topics, so I wanted to start a forum discussion specifically for developing the acoustic location system.  https://forum.openrov.com/t/acoustic-location-system

Knowing where you are when exploring and gathering images and data is clearly a vital piece of information.  GPS systems provide this on land.  Underwater, radio waves that GPS depends on to function are absorbed by water.  Acoustic signals are one of the best alternatives to transmit information for location and other data communication.  Commercial underwater modem systems exist today and are widely used in exploration, but suitable systems for low-cost small size platforms like OpenROV are not available.  This project plans to deliver such a system and is in-part inspired by university research in this area – Bridget Benson’s UCSD PhD Thesis http://haduken.ucsd.edu/papers/phd-thesis-benson.pdf is one example.
Information about 3D location in real-time is a key enabler for many scientific and exploratory needs. The technology to do this is known, but has not been scaled down for systems of this size. With location information, one could map any discoveries (sunken ships, archeological sites, and coral reefs are some examples). Maps of the ocean/lake/cave can be made with location and sensor information together. Virtual Reality and 3D gaming technology could be used to represent information about an exploration either in real-time (shared over the internet) or archived for many to explore on-line as they study results of explorations.
The technical approach is to use acoustic transmissions (ultrasonic sonar) from known locations on the surface (GPS would be used to maintain location of the sender). The ROV would have a receiver unit on board and can determine the time for the signal to arrive. That information along with depth, temperature and the on-board IMU (gyro, compass, accelerometers) would be used to determine/compute location. Location information would be available to the operator of the ROV and a graphical display could be developed (possibly in conjunction with maps of the bottom if available) to help with navigation.

Current status and near term work items:
Working designs for ultrasonic Power AMP and receiver exist and initial testing with 40KHz piezoelectric transducers has begun.  A micro-controller or processor can be used to generate a 40KHz square wave pulse which is shaped and amplified into a sine wave to drive the transducer.  Currently the amplifier produces a 27Vp-p signal.  If testing shows that this is not enough to generate a powerful enough ultrasonic pulse, a step-up transformer may be required.
The high gain (> 100), high input impedance (FET front end) receiver amplifier design includes a narrow band pass filter to allow only frequencies in the desired range to be amplified.  This receiver needs to be packaged and integrated with the ROV electronics along with a micro-controller to process the waveform (>100 samples/sec and real-time FFT processing required).  
System level: Some how the timing for the start of the pulse from each transmitter (correlated with GPS location at time of transmission) must be matched against the time the first pulse is received (any delayed echoes would be rejected).  The time of travel for the pulse would be used to calculate the distance from the transmitter (with know location) and along with depth information, water temperature, salinity (salt or fresh) and IMU data (plus and location history - dead reckoning, etc.) location would be determined. Data from a second transmitter some distance from the first would greatly help the calculation (see Virtual Long Baseline VLBL Navigation).  The transmissions and calculations should be able to be done every tenth of a second to provide timely location.
I will establish a location on GitHub for this project and start to organize existing material there.  for now, take a look at the blog https://forum.openrov.com/t/acoustic-modems-location-and-pingers.
It would speed things up if I got some help with the micro-controller software while I continue with the electronics and in the water testing.  To do: sample the signal and determine when a 40KHz pulse arrives.  Digital signal processing (DSP) analysis would be the likely approach.  Also will need to determine which micro-controller is adequate to handle this processing load.  I will collect some raw data samples using my NI VirtualBench if someone just wants to work on the DSP part.
There are a few folks already interested, and some possibility of collaboration is there.
If you are interested in getting involved, please let me know and what area(s) you think you can hep with.

I have added a folder with results from acoustic testing.  It includes the results of some initial DSP processing using MATLAB, courtesy of Frikk Solberg of Norway.

Regards,

Jim Trezzo
jtrezzo@trezco.com
