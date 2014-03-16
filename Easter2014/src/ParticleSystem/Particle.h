

namespace ParticleSystem
{
	struct Particle
	{
	public:
		ofVec2f Position;
		ofVec2f Speed; 	// velocity + direction
		float Mass;
		ofColor Color;
		float Age; 		// in seconds
		float LifeTime;	// in seconds
		bool Active;
	};
};

