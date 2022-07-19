using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class AtkCol : MonoBehaviour
{

	bool doAtk = false;//true=공격중, false = 평시
    bool animAtk=false;
    float atkSpeed;
    GameObject atckCollider;

    // Start is called before the first frame update
    void Start()
    {
        atkSpeed=0.7f;
    }

    // Update is called once per frame
    void Update()
    {
		if(Input.GetButtonDown("Attack") && !animAtk && !gameObject.transform.parent.GetComponent<Player>().GetDash())
		{	
            StartCoroutine("AttackDelay");
        }
        
    }
    
    
    //허공에 공격한 다음 공격 끝났음에도, 몬스터 안에 트리거가 들어가면 공격판정이 되는 것을 방지하기 위해 animAtk과 분리된 doAtk 변수로 판정.(animAtk은 애니메이션용, doAtk은 공격판정용)
    IEnumerator AttackDelay()
	{	
        //atkSpeed만큼 true = 공격중 판정 = 재공격 불가능	
        doAtk = true;
        animAtk=true;
		gameObject.transform.parent.GetComponent<Player>().animator.SetTrigger("animAtkTrigger");
		yield return new WaitForSeconds(atkSpeed);
        doAtk = false;
        animAtk = false;

	}

    public bool GetAtkState()
    {
        return animAtk;
    }

    void OnTriggerStay2D(Collider2D other)
	{
		//콜라이더에 닿지만 트리거 콜라이더는 예외
        //한번에 1회만 공격할 수 있게 doAtk 추가.
        //animAtk으로 판정시, atkSpeed 동안 멈춰있게 하는 기능 사용불가하여, doAtk으로 판정
        //doAtk = false가 없을 시, 한번 공격버튼으로 여러번 데미지 입혀서, 1회 공격한 후 다시 공격 버튼 눌러야 데미지 들어갈 수 있게 false로 변경.
        if(other.gameObject.tag == "HitBox" && doAtk) 
		{
            doAtk = false;

			HitBoxTrigger creture = other.gameObject.GetComponent<HitBoxTrigger>();
			creture.DamageHealth(1);			
			ScoreManager.setScore(creture.GetMonsterScore());

		}
	}

}
