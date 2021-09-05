using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class IdleColl : MonoBehaviour
{
    Rigidbody2D Idlerigid;
   	SpriteRenderer IdleSpriteRenderer;
    
    Component IdlePlayer;

    bool UnBeat;
    bool labberON;
    bool labberPlatformIn;

    // Start is called before the first frame update
    void Start()
    {
        UnBeat = false;
        Idlerigid = gameObject.transform.parent.GetComponent<Rigidbody2D>();
        IdleSpriteRenderer = gameObject.transform.parent.GetComponent<Player>().spriteRenderer;
        labberON = false;
        labberPlatformIn = false;
    }


    void OnTriggerStay2D(Collider2D other)
	{
	//	Debug.Log(other.gameObject.layer);
	//	Debug.Log(other.gameObject.tag);

        //몬스터와 접촉
		if(other.gameObject.tag == "Creature" && !other.isTrigger && !UnBeat)
		{
            Vector2 AttakedVelocity = Vector2.zero;

			float AttakedY = 0f;//1f;
            float AttakedX = 0f;//1.5f;
            if(other.gameObject.transform.position.x > gameObject.transform.parent.GetComponent<Player>().transform.position.x )
				AttakedVelocity = new Vector2(-AttakedY,AttakedX);
			else
				AttakedVelocity = new Vector2(AttakedY,AttakedX);
		   
           
            Idlerigid.AddForce(AttakedVelocity,ForceMode2D.Impulse);
				
			gameObject.transform.parent.GetComponent<Player>().PlayerHealthFunc(-1);
            if(gameObject.transform.parent.GetComponent<Player>().GetPlayerHealth() > 0)
            {
                UnBeat = true;
                StartCoroutine("UnBeatableT");
            }


            if(gameObject.transform.parent.GetComponent<Player>().GetPlayerHealth() < 1)
            {
                
            }
           
		}
        //사다리 안에서 위아래 키 누르면 true;
		if(other.gameObject.tag == "Labber")
        {
            if(Input.GetAxisRaw("Vertical") < 0)//down
                labberON = true;
            else if(Input.GetAxisRaw("Vertical") > 0)//up
                labberON = true;

        }
        //사다리 타다가 플랫폼 통과할 때 좌우 이동 제한
        if(labberON)
        {
            if(other.gameObject.tag == "Platform")
            {
                labberPlatformIn = true;
            }   
        }
            
        //end Game
        if(other.gameObject.tag == "End")
        {
			other.enabled = false;
            GameManager.EndGame();
		}
    }

    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "Labber")
            labberON = false;           
        if(other.gameObject.tag == "Platform")
            labberPlatformIn = false;
    }


    public void Damage(int damage)
    {

        if(!UnBeat)
        {
            gameObject.transform.parent.GetComponent<Player>().PlayerHealthFunc(-damage);
            if(gameObject.transform.parent.GetComponent<Player>().GetPlayerHealth() > 0)
            {
                UnBeat = true;
                StartCoroutine("UnBeatableT");
            }
            if(gameObject.transform.parent.GetComponent<Player>().GetPlayerHealth() < 1)
            {

            }
        }        
    
    }
        //피격시 무적함수
    IEnumerator UnBeatableT()
	{
		int count = 0;

		while (count <10)
		{
			if(count % 2 == 0)
				IdleSpriteRenderer.color = new Color32(255,255,255,90);
			else
				IdleSpriteRenderer.color = new Color32(255,255,255,180);
			
			yield return new WaitForSeconds(0.2f);
			count++;
		}

		IdleSpriteRenderer.color = new Color32(255,255,255,255);

        UnBeat = false;
	}

    public bool GetLabberPlatformIn()
    {
        return labberPlatformIn;
    }
    
    public bool GetLabberOn()
    {
        return labberON;
    }

}
