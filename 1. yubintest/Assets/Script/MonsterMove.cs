using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class MonsterMove : MonoBehaviour {

	public float movePower = 1f;
	public float creatureType;
	public int Health;
	public int score;

	Animator animator;
	Vector3 movement;
	int movementFlag; // 0; idle, 1;left, 2;right

	bool rightleft; //true=right, false = left
	bool isDie;
	bool isTracing;
	GameObject traceTarget;
	// Use this for initialization
	void Start () {
		animator = gameObject.GetComponentInChildren<Animator>();
		movementFlag = 0;
		rightleft = false;
		isDie = false;
		isTracing = false;
		StartCoroutine("ChageMovement");
	}
	
	IEnumerator ChageMovement()
	{
		movementFlag = Random.Range(0,3);

	//	if(movementFlag == 0)
			//animator.SetBool("isMoving",false);
	//	else
			//animator.SetBool("isMoving",true);

			
		yield return new WaitForSeconds(2f);
		StartCoroutine("ChageMovement");
	}
	
	void FixedUpdate()
	{
		Move();
	}

	void Move()
	{
		Vector3 moveVelocity = Vector3.zero;
		string dist = "";

		if(isTracing)
		{
			Vector3 playerPos = traceTarget.transform.position;

			if(playerPos.x < transform.position.x)
				dist = "Left";
			else if(playerPos.x > transform.position.x)
				dist = "Right";
		}

		else
		{
			if(movementFlag == 1)
				dist = "Left";
			else if(movementFlag == 2)
				dist = "Right";
		}



		if(dist == "Left")
		{
			moveVelocity = Vector3.left;
			transform.localScale = new Vector3(1,1,1);
		}
		else if(dist == "Right")
		{
			moveVelocity = Vector3.right;
			transform.localScale = new Vector3(-1,1,1);
		}

		transform.position += moveVelocity * movePower / 6 * Time.deltaTime;
	}


	// Update is called once per frame
	void Update () 
	{
		
	}


	void OntriggerEnter2D(Collider2D other)
	{
		if(creatureType == 0)
			return;
		if(other.gameObject.tag=="Player")
		{
			Debug.Log("trigger player");

			traceTarget = other.gameObject;
			StopCoroutine("ChageMovement");
		}
	}

	void OntriggerStay2D(Collider2D other)
	{
		isTracing = true;
		//animator.SetBool("isMoving",true);	

	}

	void OntriggerExit2D(Collider2D other)
	{
		isTracing = false;
		StartCoroutine("ChageMovment");
	}

	public void DamageHealth(int Dem)
	{		
		Health -= Dem;
		if(Health <= 0)
		{
			Die();	
		}	
	}


	public void Die()
	{
		StopCoroutine("ChangeMovement");
		isDie=true;

		SpriteRenderer renderer = gameObject.GetComponentInChildren<SpriteRenderer>();
		renderer.flipY = true;

		BoxCollider2D coll = gameObject.GetComponent<BoxCollider2D>();
		coll.enabled = false;

		Rigidbody2D rigid = gameObject.GetComponent<Rigidbody2D>();
		Vector2 dieVelocity = new Vector2(0,0.1f);
		rigid.AddForce(dieVelocity,ForceMode2D.Impulse);

		Destroy(gameObject,5f);
	}
}
