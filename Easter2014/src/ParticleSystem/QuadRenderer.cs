using UnityEngine;
using System.Collections;
using Lex;

// this is ultimately probably a base class for different types of renderers:
// billboard-quad, streamline, etc
// these will have different shaders attached, so these monobehaviours
// will be attached to different gameobjects
// note that it should be possible to have multiple renderers on one particle system
public class QuadRenderer : MonoBehaviour 
{
	public GameObject TheCamera;
	public ParticleSystemTest ParticleSystemTest;
	
	// allocate buffers for as many particles as we will need
	// don´t reallocate these all the time
	private int _nrOfParticles = 0;
	private Vector3[] _vertices;
	private Vector2[] _uvs;	
	private int[] _triangleIndices;
	
	// Use this for initialization
	void Start () 
	{
		// retrieve the nr of particles in the simulation	
		_nrOfParticles = ParticleSystemTest.ParticleManager.Particles.Length;
		
		// allocate all the buffers we need for this run
		_vertices = new Vector3[_nrOfParticles*4];
		_uvs = new Vector2[_nrOfParticles*4];		
		_triangleIndices = new int[_nrOfParticles*6];
		
	}
	
	// Update is called once per frame
	void Update () 
	{
		CreateParticles();
	}
	
	
	void CreateParticles()
	{
		MeshFilter meshFilter = GetComponent<MeshFilter>();
		if (meshFilter==null)
		{
			Debug.LogError("MeshFilter not found!");
			return;
		}
		
		Mesh mesh = meshFilter.sharedMesh;
		if (mesh == null)
		{
			meshFilter.mesh = new Mesh();
			mesh = meshFilter.sharedMesh;
		}
		mesh.Clear();


		GenerateParticlesBillboardQuads();
	
		mesh.vertices = _vertices;
		mesh.uv = _uvs;
		mesh.triangles = _triangleIndices;
		
		//mesh.RecalculateNormals();
		//mesh.RecalculateBounds();
		//mesh.Optimize();	
		
	}
	
	void GenerateParticlesBillboardQuads()
	{
		int vertexStartIndex = 0;
		int uvStartIndex = 0;		
		int triangleStartIndex = 0;
		
		for(int i=0; i<_nrOfParticles; i++)
		{
			//Vector3 pos = new Vector3(Random.value * 10f, Random.value * 10f, Random.value * 10f);
			Vector3 pos = ParticleSystemTest.ParticleManager.Particles[i].Position;
			
			vertexStartIndex = i*4;
			uvStartIndex = i*4;
			triangleStartIndex = i*6;
			
			if(i<ParticleSystemTest.ParticleManager.NrOfActiveParticles)
			{
				CreateBillboardQuad(ref _vertices, vertexStartIndex, 
									ref _uvs, uvStartIndex, 
									ref _triangleIndices, triangleStartIndex,
									pos, 5f, TheCamera.transform.right, TheCamera.transform.up);
			}
			else
				CreateDegenereteQuad(ref _vertices, vertexStartIndex, 
									ref _uvs, uvStartIndex, 
									ref _triangleIndices, triangleStartIndex,
									pos, 5f, TheCamera.transform.right, TheCamera.transform.up);
			
		}
	}
		
	// refactor: pass array of vectors, indices, start positions, view direction of camera
	public void CreateBillboardQuad(ref Vector3[] vertices, int vertexStartIndex,
									ref Vector2[] uvs, int uvStartIndex,
									ref int[] indices, int indicesStartIndex, 
									Vector3 pos, float size, Vector3 rightDirection, Vector3 upDirection)
	{
		float halfSize = size * 0.5f;
		Vector3 p0 = pos + rightDirection * -halfSize + upDirection *  halfSize;
		Vector3 p1 = pos + rightDirection *  halfSize + upDirection *  halfSize;
		Vector3 p2 = pos + rightDirection *  halfSize + upDirection * -halfSize;
		Vector3 p3 = pos + rightDirection * -halfSize + upDirection * -halfSize;
		
		vertices[vertexStartIndex+0] = p0;
		vertices[vertexStartIndex+1] = p1;
		vertices[vertexStartIndex+2] = p2;
		vertices[vertexStartIndex+3] = p3;
		
		uvs[uvStartIndex+0] = new Vector2(0, 0);
		uvs[uvStartIndex+1] = new Vector2(1, 0);
		uvs[uvStartIndex+2] = new Vector2(1, 1);
		uvs[uvStartIndex+3] = new Vector2(0, 1);
		
		indices[indicesStartIndex+0] = vertexStartIndex + 0;
		indices[indicesStartIndex+1] = vertexStartIndex + 1;
		indices[indicesStartIndex+2] = vertexStartIndex + 2;
		indices[indicesStartIndex+3] = vertexStartIndex + 0;
		indices[indicesStartIndex+4] = vertexStartIndex + 2;
		indices[indicesStartIndex+5] = vertexStartIndex + 3;
		
	}
	
	private void CreateDegenereteQuad(ref Vector3[] vertices, int vertexStartIndex,
									ref Vector2[] uvs, int uvStartIndex,
									ref int[] indices, int indicesStartIndex, 
									Vector3 pos, float size, Vector3 rightDirection, Vector3 upDirection)
	{
		vertices[vertexStartIndex+0] = Vector3.zero;
		vertices[vertexStartIndex+1] = Vector3.zero;
		vertices[vertexStartIndex+2] = Vector3.zero;
		vertices[vertexStartIndex+3] = Vector3.zero;
		
		uvs[uvStartIndex+0] = new Vector2(0, 0);
		uvs[uvStartIndex+1] = new Vector2(1, 0);
		uvs[uvStartIndex+2] = new Vector2(1, 1);
		uvs[uvStartIndex+3] = new Vector2(0, 1);
		
		indices[indicesStartIndex+0] = vertexStartIndex + 0;
		indices[indicesStartIndex+1] = vertexStartIndex + 1;
		indices[indicesStartIndex+2] = vertexStartIndex + 2;
		indices[indicesStartIndex+3] = vertexStartIndex + 0;
		indices[indicesStartIndex+4] = vertexStartIndex + 2;
		indices[indicesStartIndex+5] = vertexStartIndex + 3;
		
	}	
}
