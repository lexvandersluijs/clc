

namespace ParticleSystem
{
	class ParticleAnimator
	{
	protected:
		ParticleManager* _particleManager;

	public:
		ParticleAnimator (ParticleManager* particleManager) 
		{
			_particleManager = particleManager;
		}
		
		virtual void Update(float time, float timeStep)
		{
		}
	};
};

