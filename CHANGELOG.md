# 2019.01.31: Version 0.2
	- GUI and Audio API have been merged into IMGUI
	- Audio statistics are now visible in an IMGUI window.
	- Recording/Buffer interfaces are more flexible:
		- No limit on the number of channels
	
# 2018.07.19: Version 0.1
	- Two implementations: IMGUI and Oscilloscope
	- IMGUI is a functioning demo of the immediate-mode OpenGL GUI system
	- Oscilloscope is a simple rendering of real-time audio data.
	- Recording/Buffer interfaces have been created for PortAudio:
		- An initial threaded impl. handles audio buffers with locks