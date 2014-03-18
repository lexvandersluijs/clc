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
			_colorArray = new float[MAX_POINTS * 3 * 2]; // 3 components per color, 2 colors per line
		}
		~ParticleRenderer()
		{
			delete [] _linePoints;
			delete [] _colorArray;
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
				int startIdx = i*2;
				int endIdx = startIdx+1;

				// limit length to 10 pix
				if(length > 5.f)
				{
					delta *= 5.f/length;
				}

				_linePoints[startIdx].x = particlesArray[i].Position.x;
				_linePoints[startIdx].y = particlesArray[i].Position.y;
				_linePoints[startIdx].z = 0.f;

				_linePoints[endIdx].x = particlesArray[i].Position.x + delta.x;
				_linePoints[endIdx].y = particlesArray[i].Position.y + delta.y;
				_linePoints[endIdx].z = 0.f;

				int startIdxColor = i*3*2;
				int endIdxColor = startIdxColor+1;

				// brightness decreases linearly with age..
				float factor = 1.0f - particlesArray[i].Age/particlesArray[i].LifeTime;
				_colorArray[startIdxColor  ] = factor;
				_colorArray[startIdxColor+1] = factor;
				_colorArray[startIdxColor+2] = factor;
				_colorArray[startIdxColor+3] = factor;
				_colorArray[startIdxColor+4] = factor;
				_colorArray[startIdxColor+5] = factor;
			}

			// use smoothness, if requested:
			ofEnableSmoothing();

			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, 0, _colorArray);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), _linePoints);

			glDrawArrays(GL_LINES, 0, nrOfActiveParticles*2);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			// use smoothness, if requested:
			ofDisableSmoothing();
		}



		void DrawQuads()
		{
		}

	private:
		ParticleRenderMode _renderMode;
		ParticleManager* _particleManager;

		ofVec3f* _linePoints;
		float* _colorArray;
		int MAX_POINTS;
	};
};