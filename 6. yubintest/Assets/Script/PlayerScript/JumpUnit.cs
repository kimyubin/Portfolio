using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JumpUnit : MonoBehaviour
{

    
	Rigidbody2D jumpRigid;
	Animator animator;

    // Start is called before the first frame update
    void Start()
    {
        jumpRigid = gameObject.transform.parent.GetComponent<Player>().rigid;
    }

    void OnTriggerEnter2D(Collider2D other)
	{
		//Debug.Log(other.gameObject.layer);
		//Debug.Log(other.gameObject.tag);

		gameObject.transform.parent.GetComponent<Player>().SetFloorTouch(true);

        //점프
		if(jumpRigid.velocity.y <= 0  && !other.isTrigger && !(other.gameObject.tag == "Creature"))
			gameObject.transform.parent.GetComponent<Player>().animator.SetBool("animJumpState",false);

        //점프 & 포탈
        if(other.gameObject.layer == 8 && jumpRigid.velocity.y < 0 && (other.gameObject.name =="Up" || other.gameObject.name =="Portal Enter"))
		{

			BlockStatus block = other.gameObject.GetComponent<BlockStatus>();

			 switch(block.type)
			 {
				 case "Up":
				 	Vector2 upVelocity = new Vector2(0, 1f);
					jumpRigid.AddForce(upVelocity,ForceMode2D.Impulse);
					break;
				
				case "Portal Enter":
					Vector3 anotherPortalPs = block.portal.transform.position;
					Vector3 wrapPos = new Vector3(anotherPortalPs.x,anotherPortalPs.y+0.5f,anotherPortalPs.z);
					gameObject.transform.parent.GetComponent<Player>().transform.position = wrapPos;
					break;
			 }
		}
    }
	void OnTriggerExit2D(Collider2D other)
	{
		gameObject.transform.parent.GetComponent<Player>().SetFloorTouch(false);
	}


}
