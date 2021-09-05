using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EarthScript : MonoBehaviour
{
	public float movePower = 1f;
	public float creatureType;
	public int Health;
	public int score;
	Animator animator;
	public GameObject EarthStone;
    public float StoneDelay;


    bool rightleft ; //true=right, false = left
    bool beAttacking;
	bool isDie;

	// Use this for initialization
	void Start () 
    {
		animator = gameObject.GetComponentInChildren<Animator>();
        

        Health = 10;
        score = 1;

		isDie = false;
        rightleft = false;
        beAttacking = false;
        StartCoroutine("ChangeMove");

	}
	// Update is called once per frame
	void Update () 
	{
        if(!isDie && !beAttacking && gameObject.GetComponentInChildren<AttackRangeTriggerEarth>().GetPlayerIn())
            Attack();
	}
	void FixedUpdate()
	{
        //공격 중 정지, 딜레이 중 정지
        if(!beAttacking  && !isDie)
    		Move();
	}

    void Attack()
    {
        StartCoroutine("AttackControl");
        Instantiate(EarthStone,gameObject.transform.position,gameObject.transform.rotation);
    }
    IEnumerator AttackControl()
    {      
        beAttacking = true;//공격 중, 재공격 방지
        yield return new WaitForSeconds(StoneDelay);
        beAttacking = false;
    }


	void Move()
	{
		Vector3 moveVelocity = Vector3.zero;

        //전방이 절벽이면 방향 전환. false = 방향전환.
		if(!gameObject.GetComponentInChildren<MoveRangeTriggerEarth>().GetinMoveTrigger())
			rightleft = !rightleft;
       
        //플레이어가 공격범위 안에 들어오면, 1초 딜레이 후 방향전환
        //플레이어가 공격 범위 안에 들어오면 방향전환
        StartCoroutine("PlayerChangeDirection");

		if(rightleft)//오른쪽
		{
			moveVelocity = Vector3.right;
			transform.localScale = new Vector3(-1,1,1);
		}
		else if(!rightleft)
		{
			moveVelocity = Vector3.left;
			transform.localScale = new Vector3(1,1,1);
		}

		transform.position += moveVelocity * movePower / 6 * Time.deltaTime;
	}
    IEnumerator PlayerChangeDirection()
	{
        Vector3 playerPos = gameObject.GetComponentInChildren<TrackingRangeTriggerEarth>().GetPlayerPos();

        if(gameObject.GetComponentInChildren<TrackingRangeTriggerEarth>().GetPlayerFind())
        {
            yield return new WaitForSeconds(0.5f);

            if(transform.position.x - playerPos.x < -0.3)//플레이어가 오른쪽
                rightleft = true;
            else if(transform.position.x - playerPos.x > 0.3 )
                rightleft = false;         
        }
	}
    //플레이어를 발견 못하면 8초에 한번 방향 전환.
    IEnumerator ChangeMove()
    {
		if(!gameObject.GetComponentInChildren<TrackingRangeTriggerEarth>().GetPlayerFind())
		{
			if(rightleft)
				rightleft = false;
		    else if(!rightleft)
				rightleft = true;
		}
        yield return new WaitForSeconds(8f);
        StartCoroutine("ChangeMove");
    }



	public void Die()
	{
		isDie=true;

        animator.enabled = false;

		SpriteRenderer renderer = gameObject.GetComponentInChildren<SpriteRenderer>();
		renderer.flipY = true;

		BoxCollider2D coll = gameObject.GetComponentInChildren<IdleCollMonsterEarth>().GetComponent<BoxCollider2D>();
		coll.enabled = false;

		Rigidbody2D rigid = gameObject.GetComponent<Rigidbody2D>();
		Vector2 dieVelocity = new Vector2(0,0.1f);
		rigid.AddForce(dieVelocity,ForceMode2D.Impulse);

		Destroy(gameObject,5f);
	}

    public bool GetRightLeft()
    {
        return rightleft;
    }
    public int GetScore()
    {
        return score;
    }
    
	public void DamageHealth(int Dem)
	{
		
		Health -= Dem;
		if(Health <= 0)
		{
			Die();	
		}	
	}
}
