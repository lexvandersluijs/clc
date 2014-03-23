#pragma once
/*
This class has the settings for all creative objects in our app
It's a singleton, therefore available globally.
This isn't the cleanest OO approach, but it does allow for fast experimentation
with settings and variables, much faster than having a single GUI object that controls the
variables and passes everything along to dependent objects via function parameters.
A single new parameter added would cause many cumbersome changes in many function definitions..

Also, this settings class only depends on based openFrameworks classes, so its header
can be included before all effects, without causing things that would happen if we
would put the settings in 'ofApp.h' -> we would have to pre-declare the class, move all
implementation from header to cpp files, therefore have to manage both definition and
declaration, etc..
*/

class appSettings
{
public:

	appSettings()
	{
		inst = this;
	}
	static appSettings* inst;
	static appSettings& instance()
	{
		return *inst;
	}
	void setup()
	{
		// ---------------- initialize timeline --------------
		timeline.setup(); //registers events
		timeline.setDurationInSeconds(10); //sets time
		timeline.setLoopType(OF_LOOP_NORMAL); //turns the timeline to loop

		//add a tracks, etc
		timeline.addCurves("MyCircleRadius", ofRange(0, 200));
		timeline.addFlags("Events");

		// ------------------ intialize GUI -------------------
		gui.setup("Settings", "settings.xml", 10.f, 400.f); 
		gui.setDefaultWidth(400);

		gui.add(speedBasedGeneration.setup("Speed based generation", true));
	}
	void draw()
	{
		gui.draw();
		timeline.draw();
	}
	void update()
	{
	}
	// Control
	ofxTimeline timeline;

	ofxPanel gui;
	ofxToggle	   speedBasedGeneration;


};