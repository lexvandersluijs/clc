

namespace ParticleSystem
{

class ParticleManager 
{

private:
	Particle* _particles;
	int _nrOfParticles;
	int _nrOfActiveParticles;
	
public:
	ParticleManager(int nrOfParticles)
	{
		_nrOfParticles = nrOfParticles;
		_nrOfActiveParticles = 0;
		_particles = new Particle[nrOfParticles];
			
	}
	~ParticleManager()
	{
		delete [] _particles;
	}

	Particle* GetParticlesArray()
	{
		return _particles; 
	}

	
	// returns index of new particle that can be initialized,
	// or -1 if we are out
	int ActivateParticle()
	{
		int activatedParticleIndex = -1;
		
		// do we have a particle left that we can activate?
		if(_nrOfActiveParticles < _nrOfParticles)
		{
			activatedParticleIndex = _nrOfActiveParticles;
			++_nrOfActiveParticles;
		}
		
		return activatedParticleIndex;
	}

	int GetNrOfActiveParticles()
	{
		return _nrOfActiveParticles; 
	}

	void Update(float timeStep)
	{
		for(int i=0; i<_nrOfActiveParticles; i++)
			_particles[i].Age += timeStep;

		DeactivateZombies(); // move particles that have expired to the end of the list
	}

	void DeactivateZombies()
	{
		// move all those that have expired to the back of the 
		// list and decrease the active particles counter
		for(int i=0; i<_nrOfActiveParticles; i++)
		{
			if(_particles[i].Age > _particles[i].LifeTime)
			{
				// copy the particle at the end of our 'active' range over to the 
				// current spot, and decrease the count of active particles
				_particles[i] = _particles[_nrOfActiveParticles-1];
				--_nrOfActiveParticles;
			}
		}
	}
};

};