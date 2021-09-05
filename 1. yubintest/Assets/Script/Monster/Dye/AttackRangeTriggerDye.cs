using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackRangeTriggerDye : MonoBehaviour
{
    Vector3 playerPosition;
    bool playerIn;
    Collider2D PlyerColl;
    
    bool attack;
    // Start is called before the first frame update
    void Start()
    {
        attack = false;
    }

    // Update is called once per frame
    void Update()
    {
        if(attack)
            StartCoroutine("Attack");
    }

    IEnumerator Attack()
    {
        yield return new WaitForSeconds(1.5f);
        PlyerColl.gameObject.GetComponent<IdleColl>().Damage(1);
    }



    void OnTriggerStay2D(Collider2D other)
    {
        // 플레이어가 들어오면 플레이어의 위치를 추적해 지면과 평행하는 레이저 발사
        // 플레이어를 지속적으로 공격. 공격 대상은 플레이어 IdleCollTrigger. 
        //TriggerStay에서 damage함수 직접 실행하면, 함수가 지속적으로 호출이 안됨.(움직여야 호출) 그래서 update()로 넘겨서 실행
        if(other.gameObject.tag == "PlayerIdleCollider")
        {
            playerPosition = new Vector3(other.gameObject.transform.position.x, transform.position.y  - 0.0312f,transform.position.z);
            playerIn = true;          
            attack = true;
            PlyerColl = other;
        }   
    }
    
    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "PlayerIdleCollider")
        {
            playerIn = false;
            attack = false;
        }
    }

    public Vector3 GetPlayerPosition()
    {
        return playerPosition;
    }
    public bool GetPlayerIn()
    {
        return playerIn;
    }


}
