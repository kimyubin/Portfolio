using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class Player : MonoBehaviour {

	public float movePower;
	public float jumpPower;
	public int MaxHealth = 100;
	int PlayerHealth = 100;
	bool LifeOrDeath = true;
	
	//보고있는 방향
	bool rightLeft;//right = true, left = false
	// 게임 내부 최대 제한 속도(종단속도 겸)
	float terminalVelocity; 
	
	//바닥에 닿고 있는 지
	bool floorTouch;

	//대쉬를 위한 시간 저장
	float leftPreTime;
	float rightPreTime;
	float leftDashTime;
	float rightDashTime;

	bool doDash;
	
	//이전 프레임에서 사다리를 타고있었는지 저장. 사다리에서 벗어났는지 확인용	
	bool prevLabber;
	//사다리 + 플랫폼 통과중에 좌우 이동 제한. ture = 제한


	public SpriteRenderer spriteRenderer;
	public Rigidbody2D rigid;
	public Animator animator;


	bool jumpingState;
	

	// Use this for initialization
	void Start () 
	{
		rigid = gameObject.GetComponent<Rigidbody2D>();
		animator = gameObject.GetComponentInChildren<Animator>();
		spriteRenderer = gameObject.GetComponentInChildren<SpriteRenderer>();
		terminalVelocity = 15f;
		jumpingState = false;

		rightLeft = true;
		doDash  = false;
		prevLabber = false;
	}
	
	// Update is called once per frame
	void Update () 
	{
		MoveAnimation();
		JumpAnimation();
		LabberAnimation();
		TerminalVelocityFunc();

		//앉기 + 아래 = 플랫폼 통과
		if(Input.GetButton("Sit"))
			if(Input.GetKeyDown("down"))
				Physics2D.IgnoreLayerCollision(8,10,true);
	////	if(gameObject.GetComponentInChildren<IdleColl>().GetPlatformOut())
		//	Physics2D.IgnoreLayerCollision(8,10,false);

		//death
		if(PlayerHealth <= 0)
		{
			if(LifeOrDeath)
				Die();
			return;
		}

	}
	void FixedUpdate()
	{
		//dash. 좌우 화살표 누르는 간격 측정해서 일정 시간 미만이면 대쉬 판정.
		if(Input.GetKeyDown(KeyCode.RightArrow))
		{
			rightPreTime = rightDashTime;
			rightDashTime = Time.time;	
		}			
		if(Input.GetKeyDown(KeyCode.LeftArrow))
		{
			leftPreTime = leftDashTime;
			leftDashTime = Time.time;		
		}

		Move();
		Jump();
		Labber();
		if(PlayerHealth <=0)
			return;
	}

//Update
	void MoveAnimation()
	{
		//move animation
		//atkcol.doAtk이 true=공격 중이면 정지.	
		if(Input.GetAxisRaw("Horizontal") == 0 || gameObject.GetComponentInChildren<AtkCol>().GetAtkState() || gameObject.GetComponentInChildren<SwordWaveAtk>().GetWaveState())
		{
			animator.SetBool("animMoving",false);
			animator.SetBool("animDashState",false);

		}
		else if(Input.GetAxisRaw("Horizontal") < 0)
		{
			rightLeft = false;
			animator.SetInteger("Direction",-1); //left
			if(doDash)
				animator.SetBool("animDashState",true);
			else
				animator.SetBool("animMoving",true);
		}
		else if(Input.GetAxisRaw("Horizontal") > 0)
		{
			rightLeft = true;
			animator.SetInteger("Direction",1); //right
			if(doDash)
				animator.SetBool("animDashState",true);
			else
				animator.SetBool("animMoving",true);
		}
	}

	void JumpAnimation()
	{
		//jump animation
		if(Input.GetButtonDown("Jump") && !animator.GetBool("animJumpState"))
		{			
			jumpingState = true;
			animator.SetBool("animJumpState",true);
			animator.SetTrigger("animJumpTrigger");
		}		
	}

	void LabberAnimation()
	{		
		//labber animation
		if(gameObject.GetComponentInChildren<IdleColl>().GetLabberOn())
		{
            if((Input.GetAxisRaw("Vertical") == 0) && (rigid.gravityScale == 0))//stay
            {		
				animator.speed = 0f;
            }
            else if((Input.GetAxisRaw("Vertical") < 0) || (Input.GetAxisRaw("Vertical") > 0))//down // up
            {
				animator.SetBool("animLabberState",true);
				animator.SetTrigger("animLabberTrigger");
				animator.speed = 1f;


				Physics2D.IgnoreLayerCollision(8,10,true);
				rigid.gravityScale = 0f; 
                rigid.velocity = new Vector2(rigid.velocity.x,0);//중력 가속도 누적 방지

            }
           
		   // 움직이는 것 외에 다른 동작하면 사다리에서 탈락
			if(Input.anyKeyDown)
			{
				if(!(Input.GetAxisRaw("Vertical") > 0) && !(Input.GetAxisRaw("Vertical") < 0))
				{
					if(!(Input.GetAxisRaw("Horizontal") > 0) && !(Input.GetAxisRaw("Horizontal") < 0 ))
					{
						animator.SetBool("animLabberState",false);
						rigid.gravityScale = 1f;
						animator.speed = 1f;
						Physics2D.IgnoreLayerCollision(8,10,false);
					}
				}
			} 
        }
		else if(!gameObject.GetComponentInChildren<IdleColl>().GetLabberOn())
		{
			Physics2D.IgnoreLayerCollision(8,10,false);
			animator.speed = 1f;
		}

		//사다리에서 벗어났을때
		if(!gameObject.GetComponentInChildren<IdleColl>().GetLabberOn())
		{	
			//현재 프레임에서 사다리 안에 있지 않지만, 바로 직전 프레임에서 사다리 안에 있었으면 = 사다리에서 벗어나면
			if(prevLabber)
			{
				animator.SetBool("animLabberState",false);
				rigid.gravityScale = 1f;
				animator.speed = 1f;
				Physics2D.IgnoreLayerCollision(8,10,false);

			}
		}
		prevLabber = gameObject.GetComponentInChildren<IdleColl>().GetLabberOn();
	}
	void TerminalVelocityFunc()
	{
		//종단속도. 콜라이더 관통 현상 방지용
		if(rigid.velocity.y < -terminalVelocity)
			rigid.velocity = new Vector2(rigid.velocity.x,-terminalVelocity);
		//이하 비정상적인 속도 증가로 인한 콜라이더 관통현상 방지용
		if(rigid.velocity.y > terminalVelocity)
			rigid.velocity = new Vector2(rigid.velocity.x,terminalVelocity);
		if(rigid.velocity.x < -terminalVelocity)
			rigid.velocity = new Vector2(-terminalVelocity,rigid.velocity.y);
		if(rigid.velocity.x > terminalVelocity)
			rigid.velocity = new Vector2(terminalVelocity,rigid.velocity.y);
	}

	//FixedUpdate
	void Move()
	{
		Vector3 moveVelocity = Vector3.zero;
		//atkcol.doAtk이 false=평시면 움직임.
		if(!gameObject.GetComponentInChildren<AtkCol>().GetAtkState() && !gameObject.GetComponentInChildren<SwordWaveAtk>().GetWaveState())
		{
			float tempDashPower = 1f;
			float dashTime = 0.7f;
			doDash = false;
			bool inLabberBool = GetComponentInChildren<IdleColl>().GetLabberPlatformIn();

			//사다리 타는 중이면서 플랫폼 안에 있으면 좌우 이동 금지.
			if((Input.GetAxisRaw("Horizontal") < 0) && !inLabberBool)
			{
				tempDashPower = 1f;
				//dash
				if((leftDashTime - leftPreTime) < dashTime)
				{
					tempDashPower = 1.5f;
					doDash = true;
				}
				moveVelocity = Vector3.left;
					transform.localScale = new Vector3(-1,1,1);	
				}
			else if((Input.GetAxisRaw("Horizontal") > 0) && !inLabberBool)
			{
				tempDashPower = 1f;
				//dash
				if((rightDashTime - rightPreTime) < dashTime)
				{
					tempDashPower = 1.5f;
					doDash = true;
				}
				moveVelocity = Vector3.right;
				transform.localScale = new Vector3(1,1,1);

			}				
		
			transform.position += moveVelocity * movePower * Time.deltaTime * tempDashPower;			
		}
	}

	void Jump()
	{

		if(!jumpingState)
			return;

		rigid.velocity = Vector2.zero;

		Vector2 jumpVelocity = new Vector2(0,jumpPower);
		rigid.AddForce(jumpVelocity, ForceMode2D.Impulse);
		
		jumpingState = false;
	}
	void Labber()
	{
		Vector3 moveVelocity = Vector3.zero;
		//atkcol.doAtk이 false=평시면 움직임.
		if(gameObject.GetComponentInChildren<IdleColl>().GetLabberOn())
		{
			if(!gameObject.GetComponentInChildren<AtkCol>().GetAtkState() && !gameObject.GetComponentInChildren<SwordWaveAtk>().GetWaveState())
			{
				if(Input.GetAxisRaw("Vertical") < 0)
				{
					moveVelocity = Vector3.down;				
				}
				else if(Input.GetAxisRaw("Vertical") > 0)
				{
					moveVelocity = Vector3.up;	
				}		
				transform.position += moveVelocity * movePower * Time.deltaTime;			
		}


		}
	}


	void Die()
	{
		LifeOrDeath = false;
		rigid.velocity = Vector2.zero;
		animator.Play("Die");

		Vector2 dieVelocity = new Vector2(0, 1.5f);
		rigid.AddForce (dieVelocity, ForceMode2D.Impulse);

		Invoke("RestartStage", 1f);
		
	}

	void RestartStage()
	{
		Time.timeScale = 0f;
		GameManager.EndGame();
	}
	public void PlayerHealthFunc(int H)
	{
		Debug.Log(PlayerHealth);
		PlayerHealth += H;
	}
	public int GetPlayerHealth()
	{
		return PlayerHealth;
	}

	public bool GetDash()
	{
		return doDash;
	}
	public void SetDash(bool tempDash)
	{
		doDash = tempDash;
	}

	public bool GetRightLeft()
	{
		return rightLeft;
	}

	public bool GetFloorTouch()
	{
		return floorTouch;
	}
	public void SetFloorTouch(bool temp)
	{
		floorTouch = temp;
	}





	void OnTriggerStay2D(Collider2D other)
	{
	}
	
}
