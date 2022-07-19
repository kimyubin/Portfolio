using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SwordWave : MonoBehaviour
{
    public float waveSpeed ;
    bool rightLeft;
    // Start is called before the first frame update
    void Start()
    {
        // 캐릭터가 바라보는 방향으로 애니메이션 방향 전환.
        rightLeft = GameObject.FindWithTag("Player").GetComponent<Player>().GetRightLeft();
        if(!rightLeft)
			transform.localScale = new Vector3(-1,1,1);	            
    }

    // Update is called once per frame
    void Update()
    {
        //캐릭터가 바라보는 방향으로 발사.
        if(rightLeft)
            transform.Translate(Vector2.right * Time.deltaTime * waveSpeed);
        else
            transform.Translate(Vector2.left * Time.deltaTime * waveSpeed);
           
        
        Destroy(gameObject,5);
        
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.tag == "HitBox") 
		{
			HitBoxTrigger creture = other.gameObject.GetComponent<HitBoxTrigger>();
			creture.DamageHealth(1);			
			ScoreManager.setScore(creture.GetMonsterScore());
            Destroy(gameObject);
		}
        if(other.gameObject.tag == "Platform" && !other.isTrigger) 
            Destroy(gameObject);
        if(other.gameObject.tag == "Wall" && !other.isTrigger) 
            Destroy(gameObject);
    }
    

}
