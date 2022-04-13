using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WallJump : MonoBehaviour
{
    Rigidbody2D rigid;
    public float fallsVelocity;
    bool holdButton;
    bool stayBool;
    Vector3 currentPosition;
    Vector3 stayPosition;
    Vector3 enterPosition;
    Transform trasnform;

    // Start is called before the first frame update
    void Start()
    {
        holdButton = false;
        stayBool = false;
    }

    // Update is called once per frame
    void Update()
    {
        rigid = gameObject.transform.parent.GetComponent<Player>().rigid;
        trasnform = gameObject.transform.parent.GetComponent<Player>().rigid.transform; 

        if(!Input.GetButton("Hold"))
        {
            rigid.gravityScale = 1f; 
        }  
        else if(stayBool)
        {
            rigid.gravityScale = 0f; 
            rigid.velocity = new Vector2(rigid.velocity.x,0);//중력 가속도 누적 방지
        }
        
    }


    void OnTriggerStay2D(Collider2D other)
	{
		if(rigid.velocity.y < 0  && !other.isTrigger && !(other.gameObject.tag == "Creature"))
        {            
            //점프 직후 벽에 붙어있으면. 점프 후 추락상태로 애니메이션 정지됨을 방지.
            gameObject.transform.parent.GetComponent<Player>().animator.SetBool("animJumpState",false);

            if(Input.GetButton("Hold"))
            {
                stayBool = true;
                stayPosition = trasnform.position;
            }
            else
            {
                stayBool = false;
                //벽에 붙어만 있으면 천천히 내려옴. 
                if(rigid.velocity.y < -fallsVelocity)
                    rigid.velocity = new Vector2(rigid.velocity.x,-fallsVelocity);
            }                  

        }       

	
    }
    
    void OnTriggerExit2D(Collider2D other)
    {
        //벽에서 떨어지면 홀드 해제
        stayBool = false;               
        rigid.gravityScale = 1f; 
    }
}
