using UnityEngine;
using System.Collections.Generic;
using Lex;

public class ParticleSystemTest : MonoBehaviour 
{
	// also a gameObject, present in the scene
	public QuadRenderer QuadRenderer;
	
	public ParticleManager ParticleManager;
	
	private ParticleGenerator Generator = new ParticleGenerator(); // right now, a single simple generator.. can be a collection
	
	private List<Animator> _animators = new List<Animator>();
	
	// Use this for initialization
	void Awake () 
	{
		// create a pool of particles 
		ParticleManager = new ParticleManager(2000);
	
		// create an object that will activate particles from the collection in 
		// a certain location, and with the other properties set
		Generator.ParticleManager = ParticleManager;
	
		// create a way to animate these particles
		SimulationAnimator simAnim = new SimulationAnimator();
		//simAnim.ForceFields.Add(new PointForceField(Vector3.zero, 0.5f));
		//simAnim.ForceFields.Add(new PointForceField(new Vector3(10f, 10f, 0f), -1.0f));
		simAnim.ForceFields.Add(new VortexForceField(Vector3.zero, 0.5f));
		simAnim.ParticleManager = ParticleManager;
		_animators.Add(simAnim);
	}
	
	// Update is called once per frame
	void Update () 
	{
		float totalTime = Time.timeSinceLevelLoad;
		float deltaTime = Time.deltaTime;
	
		// first clean up all dead particles
		ParticleManager.DeactivateZombies();
		
		// then generate new ones
		Generator.Generate(deltaTime);

		// then compute a new state for all the existing ones		
		foreach(Animator anim in _animators)
		{
			anim.Update(totalTime, deltaTime);
		}
	}
	
	void OnGUI()
	{
		GUI.Label(new Rect(10, 10, 200, 30), "Nr of active particles: " + ParticleManager.NrOfActiveParticles);
	}
}
