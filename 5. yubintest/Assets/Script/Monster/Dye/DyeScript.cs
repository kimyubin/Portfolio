using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DyeScript : MonoBehaviour
{
	public float movePower = 1f;
	public float creatureType;
	public int Health;
	public int score;
	Animator animator;

    LineRenderer lineRend;
    Vector3 laserStart;
    Vector3 laserDestination;

    bool rightleft ; //true=right, false = left
    bool beAttacking;
    int AttackDeleyInt; // 0 = normal, true = 1 = 딜레이중, false = 2 = 공격 가능
	bool isDie;

	GameObject traceTarget;
	// Use this for initialization
	void Start () 
    {
		animator = gameObject.GetComponentInChildren<Animator>();
        lineRend = GetComponent<LineRenderer>();
        lineRend.enabled = false;
        lineRend.useWorldSpace = true;

        lineRend.startColor = Color.red;;
        lineRend.endColor = Color.red;;

        Health = 10;
        score = 1;

		isDie = false;
        rightleft = false;
        beAttacking = false;
        AttackDeleyInt = 0;
        StartCoroutine("ChangeMove");

	}
	// Update is called once per frame
	void Update () 
	{
        if(gameObject.GetComponentInChildren<AttackRangeTriggerDye>().GetPlayerIn() && !isDie && !beAttacking)
            StartCoroutine("AttackDeley");
        
        if(AttackDeleyInt == 2)
            Attack();
	}
	void FixedUpdate()
	{
        //공격 중 정지, 딜레이 중 정지
        if(!beAttacking  && !isDie && (AttackDeleyInt == 0))
    		Move();
	}

    void Attack()
    {
        //공격 범위 안에 들어오면
        if(gameObject.GetComponentInChildren<AttackRangeTriggerDye>().GetPlayerIn())
        {
            lineRend.startWidth = 0.05f;
            lineRend.endWidth = 0.05f;
            beAttacking = true;            
            lineRend.enabled = true;
            animator.SetBool("Walk",false);
            animator.SetTrigger("Attack");
        }
        else
        {
            beAttacking = false;
            AttackDeleyInt = 0;
            lineRend.enabled = false;
            animator.SetBool("Walk",true);
        }
    
        if(rightleft)
        {
            laserStart = new Vector3(transform.position.x + 0.3232f,transform.position.y - 0.0312f,transform.position.z);
            if(gameObject.GetComponentInChildren<AttackRangeTriggerDye>().GetPlayerIn())
                laserDestination = gameObject.GetComponentInChildren<AttackRangeTriggerDye>().GetPlayerPosition();
            else
                laserDestination = new Vector3(transform.position.x + 100f, transform.position.y,transform.position.z);
        }
        else
        {
            laserStart = new Vector3(transform.position.x - 0.3232f,transform.position.y - 0.0312f,transform.position.z);
            if(gameObject.GetComponentInChildren<AttackRangeTriggerDye>().GetPlayerIn())
                laserDestination = gameObject.GetComponentInChildren<AttackRangeTriggerDye>().GetPlayerPosition();
            else
                laserDestination = new Vector3(transform.position.x - 100f, transform.position.y,transform.position.z);
        }
        
        lineRend.SetPosition(0,laserStart);        
        lineRend.SetPosition(1,laserDestination);
    }
    IEnumerator AttackDeley()
    {
        animator.SetTrigger("Attack");

        lineRend.startWidth = 0.02f;
        lineRend.endWidth = 0.02f;
        

        AttackDeleyInt = 1;

        if(rightleft)
        {
            laserStart = new Vector3(transform.position.x + 0.3232f,transform.position.y - 0.0312f,transform.position.z);
            laserDestination = new Vector3(transform.position.x + 100f, transform.position.y- 0.0312f,transform.position.z);
        }
        else
        {
            laserStart = new Vector3(transform.position.x - 0.3232f,transform.position.y - 0.0312f,transform.position.z);
            laserDestination = new Vector3(transform.position.x - 100f, transform.position.y- 0.0312f,transform.position.z);

        }
        lineRend.SetPosition(0,laserStart);        
        lineRend.SetPosition(1,laserDestination);  
              
        yield return new WaitForSeconds(0.5f);
        lineRend.enabled = true;
        yield return new WaitForSeconds(0.5f);
        lineRend.enabled = false;
        yield return new WaitForSeconds(0.5f);
        AttackDeleyInt = 2;
        beAttacking = true;
        StopCoroutine("AttackDeley");
    }


	void Move()
	{
		Vector3 moveVelocity = Vector3.zero;

        //전방이 절벽이면 방향 전환. false = 방향전환.
		if(!gameObject.GetComponentInChildren<MoveRangeTriggerDye>().GetinMoveTrigger())
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
        Vector3 playerPos = gameObject.GetComponentInChildren<TrackingRangeTriggerDye>().GetPlayerPos();

        if(gameObject.GetComponentInChildren<TrackingRangeTriggerDye>().GetPlayerFind())
        {
            yield return new WaitForSeconds(1f);

            if(transform.position.x - playerPos.x < -0.3)//플레이어가 오른쪽
                rightleft = true;
            else if(transform.position.x - playerPos.x > 0.3 )
                rightleft = false;         
        }
	}
    //플레이어를 발견 못하면 8초에 한번 방향 전환.
    IEnumerator ChangeMove()
    {
		if(!gameObject.GetComponentInChildren<TrackingRangeTriggerDye>().GetPlayerFind())
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
        lineRend.enabled = false;

		SpriteRenderer renderer = gameObject.GetComponentInChildren<SpriteRenderer>();
		renderer.flipY = true;

		BoxCollider2D coll = gameObject.GetComponentInChildren<IdleCollMonsterDye>().GetComponent<BoxCollider2D>();
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
