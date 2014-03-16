namespace ParticleSystem
{
	enum ParticleRenderMode { Points, Lines, Quads };

	class ParticleRenderer
	{

	public:
		ParticleRenderer(ParticleManager* manager)
		{
			_particleManager = manager;
			_renderMode = ParticleRenderMode::Lines;
		}

		void SetRenderMode(ParticleRenderMode renderMode)
		{
		}

		void Draw(float size)
		{
			switch(_renderMode)
			{
			case ParticleRenderMode::Points:
				DrawPoints();
				break;
			case ParticleRenderMode::Lines:
				DrawLines(size);
				break;
			case ParticleRenderMode::Quads:
				DrawQuads();
				break;

			}
		}
		void DrawPoints()
		{
		}
		void DrawLines(float width)
		{
			// for all active particles in manager
			// add vertices + other info to VBO
			// push VBO to GPU

			ofSetLineWidth(width);
			Particle* particlesArray = _particleManager->GetParticlesArray();
			for(int i=0; i<_particleManager->GetNrOfActiveParticles(); i++)
			{
				ofVec2f delta = particlesArray[i].Speed;
				float length = delta.length();

				// limit length to 10 pix
				if(length > 10.f)
				{
					delta *= 10.f/length;
				}

				ofLine(particlesArray[i].Position.x, 
					   particlesArray[i].Position.y, 
					   particlesArray[i].Position.x + delta.x, 
					   particlesArray[i].Position.y + delta.y);
			}
		}
		void DrawQuads()
		{
		}

	private:
		ParticleRenderMode _renderMode;
		ParticleManager* _particleManager;
	};
};