using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackRangeTriggerEarth : MonoBehaviour
{
    Vector3 playerPosition;
    bool playerIn;    
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }
    void OnTriggerStay2D(Collider2D other)
    {
        if(other.gameObject.tag == "PlayerIdleCollider")
        {
            playerPosition = other.gameObject.transform.position;
            EarthStone.SetPlayerPosition(playerPosition);// 현재 플레이어 위치를 static함수로 저장. 아직 생성이 안된 객체라 클래스에 우선 저장하고, 간편하게 접근하기 위해 Static으로 구현.
            playerIn = true;          
        }   
    }
    
    void OnTriggerExit2D(Collider2D other)
    {
        if(other.gameObject.tag == "PlayerIdleCollider")
        {
            playerIn = false;
        }
    }

    public Vector3 GetPlayerPosition()
    {
        Debug.Log("fds");
        return playerPosition;
    }
    public bool GetPlayerIn()
    {
        return playerIn;
    }

}
