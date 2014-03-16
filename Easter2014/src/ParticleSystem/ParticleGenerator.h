
namespace ParticleSystem
{

// The system consists of the simulation, plus generators, plus animators, plus visualization info, etc
class ParticleGenerator 
{
public:
	ParticleManager* _particleManager;
	
private:
	float _generationRate; // particles / second
	float _elapsedTime;
	float _particleQuotaRemainder;
	
public:
	ParticleGenerator(ParticleManager* particleManager)
	{
		_particleManager = particleManager;

		_generationRate = 50;
		_elapsedTime = 0;
		_particleQuotaRemainder = 0;
	}
	int Generate(float timeStep, ofVec2f pos)
	{
		// add timeStep to elapsedtime 
		// if the generationrate (particles/sec) multiplied by the elapsed time > 1 (nr of particles)
		// activate this number particles, keep the ´leftover´ quota, and reset the elapsedTime
		
		// more advanced: first check with all generators how many they want to generate
		// then give each a certain part of what is allowed, based on ratios
		
		_elapsedTime += timeStep;
		float allowedCount = _particleQuotaRemainder + _generationRate * _elapsedTime;
		float roundedCount = floor(allowedCount);		
		if(roundedCount > 0)
		{
			_particleQuotaRemainder = allowedCount - roundedCount;
			_elapsedTime = 0.f;
		}
		
		int generatedCount = 0;
		int intCount = (int)roundedCount;
		for(int i=0; i<intCount; i++)
		{
			int particleIndex = _particleManager->ActivateParticle();
			
			if(particleIndex != -1)
			{
				// initialize particle in correct spot
				// right now..  simply in the vicinity of the origig
				Particle* particleArray = _particleManager->GetParticlesArray();
				particleArray[particleIndex].Position = pos + ofVec2f(RandomCentered(5.f), RandomCentered(5.f));
				particleArray[particleIndex].Speed = ofVec2f::zero();
				particleArray[particleIndex].LifeTime = 3.0f;
				particleArray[particleIndex].Mass = 2.0f;
				particleArray[particleIndex].Age = 0.0f;

				++generatedCount;
			}
			else
			{
				// out of particles
				break;
			}
		}
		return generatedCount;
	}

	float RandomCentered(float scale)
	{
		return ofRandomf() * scale; // ofRandomF is between -1 and 1
	}

};

};