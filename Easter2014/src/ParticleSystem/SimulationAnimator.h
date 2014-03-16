namespace ParticleSystem
{

class SimulationAnimator : public ParticleAnimator
{
	
private:
	std::vector<ForceField*> _forceFields;
	float _frictionFactor;
	
public:
	SimulationAnimator(ParticleManager* particleManager) : ParticleAnimator(particleManager)
	{
		_frictionFactor = 0.1f;
	}
	~SimulationAnimator()
	{
		for(std::vector<ForceField*>::iterator it = _forceFields.begin(); it != _forceFields.end(); ++it) 
		{
			delete (*it);
		}
	}
	void AddForceField(ForceField* ff)
	{
		_forceFields.push_back(ff);
	}
	virtual void Update(float time, float timeStep)
	{
	
		// for all particles
		// collect forces, apply resultant force
		// perhaps collide
		// apply age-check. if dead, move to end of buffer  (also: if dead, a degenerate representation will (not) be rendered)
		
		ofVec2f totalForce = ofVec2f::zero();
		
		Particle* particlesArray = _particleManager->GetParticlesArray();
		for(int i=0; i<_particleManager->GetNrOfActiveParticles(); i++)
		{

			for(std::vector<ForceField*>::iterator it = _forceFields.begin(); it != _forceFields.end(); ++it) 
			{
				totalForce += (*it)->ComputeForce(&(particlesArray[i]));
			}
			
			// take the friction into account. this works in the opposite direction 
			// of the particle´s movement, i.e. its speed
			totalForce -= (particlesArray[i].Speed * _frictionFactor);
			
			// F = m*a -> a = F/m
			ofVec2f accel = totalForce * (1.f / particlesArray[i].Mass);
			
			// change in speed (vector) based on integration of acceleration
			ofVec2f deltaSpeed = accel * timeStep;
			
			ofVec2f newSpeed = particlesArray[i].Speed + deltaSpeed;
			particlesArray[i].Speed = newSpeed;
	
			ofVec2f deltaPos = newSpeed * timeStep;
			
			float distanceTravelled = deltaPos.length();
			
			// change in position based on integration of velocity
			particlesArray[i].Position += deltaPos;			
			
		}
		
	}

};
	
};
