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

			MAX_POINTS = 100000;
			_linePoints = new ofVec3f[MAX_POINTS];
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
		void DrawLines_old(float width)
		{
			// for all active particles in manager
			// add vertices + other info to VBO
			// push VBO to GPU

			ofSetColor(255, 255, 255, 255);
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

		void DrawLines(float width)
		{
			// for all active particles in manager
			// add vertices + other info to VBO
			// push VBO to GPU

			ofSetColor(255, 255, 255, 255);
			ofSetLineWidth(width);

			int nrOfActiveParticles =_particleManager->GetNrOfActiveParticles();

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

				_linePoints[i*2].x = particlesArray[i].Position.x;
				_linePoints[i*2].y = particlesArray[i].Position.y;
				_linePoints[i*2].z = 0.f;

				_linePoints[i*2+1].x = particlesArray[i].Position.x + delta.x;
				_linePoints[i*2+1].y = particlesArray[i].Position.y + delta.y;
				_linePoints[i*2+1].z = 0.f;
			}

			// use smoothness, if requested:
			//if (bSmoothHinted) startSmoothing();

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &_linePoints[0].x);
			glDrawArrays(GL_LINES, 0, nrOfActiveParticles*2);

			// use smoothness, if requested:
			//if (bSmoothHinted) endSmoothing();
		}



		void DrawQuads()
		{
		}

	private:
		ParticleRenderMode _renderMode;
		ParticleManager* _particleManager;

		ofVec3f* _linePoints;
		int MAX_POINTS;
	};
};