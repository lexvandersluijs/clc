namespace ParticleSystem
{
	static const float EPSILON = 0.0001f;

class ForceField 
{
protected:

public:
	// function that computes the new position and speed of a particle based on its current state
	// for a forcefield, the current position is the most important

	ForceField()
	{
	}

	virtual ofVec2f ComputeForce(Particle* particle)
	{
		return ofVec2f::zero();
	}
};

class PointForceField : public ForceField
{
	// positive factor pushes out, negative pulls in
	
private:
	ofVec2f _center;
	float _factor;
	
public:
	PointForceField(ofVec2f center, float factor)
	{
		_factor = 1.0f;
		_center = center;
		_factor = factor;
	}
	virtual ofVec2f ComputeForce(Particle* particle)
	{
		ofVec2f force = ofVec2f::zero();
		ofVec2f delta = particle->Position - _center;
		
		// FF wants to move particle in a certain direcion
		
		// the simulation will collect all forces, then compute a resultant force,
		// and apply it to the particle
		
		float l = delta.length();
		if(l > EPSILON)
		{
			force = delta * (_factor / l);
		}
		
		return force;
	}
};
	
class VortexForceField : public ForceField
{
	// positive factor pushes out, negative pulls in
	
private:
	ofVec2f _center;
	float _factor;
	
	// possible addition: specify the axis around which the forces rotate
	
public:
	VortexForceField(ofVec2f center, float factor)
	{
		_factor = 1.0f;

		_center = center;
		_factor = factor;
	}
	virtual ofVec2f ComputeForce(Particle* particle)
	{
		ofVec2f force = ofVec2f::zero();
		ofVec2f delta = particle->Position - _center;
		
		// to get a ´rotation´ ForceField, we take the direction from the FF center
		// and take the perpendicular. but first, normalize.
		float l = delta.length();
		if(l > EPSILON)
		{
			delta *= (1.0f / l);
			force.x = -delta.y * _factor;
			force.y =  delta.x * _factor;
		}
		
		return force;
	}
};

};