namespace ParticleSystem
{

class FluidSimulationAnimator : public ParticleAnimator
{
	
private:
	ofFloatPixels& _velocityMap;
	float _frictionFactor;
	int frameCount;
	float _screenToFluidScale;

public:
	FluidSimulationAnimator(ParticleManager* particleManager, ofFloatPixels& map, float screenToFluidScale) : 
		ParticleAnimator(particleManager), 
		_velocityMap(map)
	{
		_frictionFactor = 0.1f;
		_screenToFluidScale = screenToFluidScale;
		frameCount = 0;
	}
	~FluidSimulationAnimator()
	{

	}

	virtual void Update(float time, float timeStep)
	{
		++frameCount;

		if(frameCount < 2) // wait until velocitybuffer is filled..
			return;

		// for all particles
		// collect forces, apply resultant force
		// perhaps collide
		// apply age-check. if dead, move to end of buffer  (also: if dead, a degenerate representation will (not) be rendered)
		
		int width = _velocityMap.getWidth();
		int height = _velocityMap.getHeight();

		Particle* particlesArray = _particleManager->GetParticlesArray();
		for(int i=0; i<_particleManager->GetNrOfActiveParticles(); i++)
		{
			// look up the speed of the particle in the velocity map
			// we can use the x and y value as the index, since the mapping is 1:1
			// CLAMP, because we can only look up a value if the particle is still inside the simulation area..
			int x = CLAMP((int)particlesArray[i].Position.x * _screenToFluidScale, 0, width-1);
			int y = CLAMP((int)particlesArray[i].Position.y * _screenToFluidScale, 0, height-1);
			//int index = y * _velocityMap.getWidth() + x;

			ofFloatColor c = _velocityMap.getColor(x, y); //..getPixels()+(index*3); 
			particlesArray[i].Speed.x = c.r * 10.f; // assume x and y velocity are stored in the R and G channels
			particlesArray[i].Speed.y = c.g * 10.f;
	
			ofVec2f deltaPos = particlesArray[i].Speed * timeStep;
			
			float distanceTravelled = deltaPos.length();
			
			// change in position based on integration of velocity
			particlesArray[i].Position += deltaPos;			
			
		}
		
	}

};
	
};
