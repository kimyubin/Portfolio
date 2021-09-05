using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DashAtk : MonoBehaviour
{
    
	bool doDashAtk = false;//true=공격중, false = 평시
    bool animDashAtk=false;
    float dashAtkSpeed;
    GameObject dashAtkCollider;

    // Start is called before the first frame update
    void Start()
    {
        dashAtkSpeed=0.7f;
    }

    // Update is called once per frame
    void Update()
    {
		if(Input.GetButtonDown("Attack") && !animDashAtk && gameObject.transform.parent.GetComponent<Player>().GetDash())
		{	
            StartCoroutine("DashAttackDelay");
        }
      
    }
    
    
    //허공에 공격한 다음 공격 끝났음에도, 몬스터 안에 트리거가 들어가면 공격판정이 되는 것을 방지하기 위해 animDashAtk과 분리된 doDashAtk 변수로 판정.(animDashAtk은 애니메이션용, doDashAtk은 공격판정용)
    IEnumerator DashAttackDelay()
	{	
        //dashAtkSpeed만큼 true = 공격중 판정 = 재공격 불가능	
        doDashAtk = true;
        animDashAtk=true;
		gameObject.transform.parent.GetComponent<Player>().animator.SetTrigger("animDashAtkTrigger");
		yield return new WaitForSeconds(dashAtkSpeed);
        doDashAtk = false;
        animDashAtk = false;

	}

    public bool AtackBoolFunc()
    {
        return animDashAtk;
    }

    void OnTriggerEnter2D(Collider2D other)
	{
		//콜라이더에 닿지만 트리거 콜라이더는 예외
        //한번에 1회만 공격할 수 있게 doDashAtk 추가.
        //animDashAtk으로 판정시, dashAtkSpeed 동안 멈춰있게 하는 기능 사용불가하여, doDashAtk으로 판정
        //doDashAtk = false가 없을 시, 한번 공격버튼으로 여러번 데미지 입혀서, 1회 공격한 후 다시 공격 버튼 눌러야 데미지 들어갈 수 있게 false로 변경.
        if(other.gameObject.tag == "HitBox" && doDashAtk) 
		{
            //doDashAtk = false;

			HitBoxTrigger creture = other.gameObject.GetComponent<HitBoxTrigger>();
			creture.DamageHealth(1);			
			ScoreManager.setScore(creture.GetMonsterScore());

		}

	}
}
