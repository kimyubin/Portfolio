using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HitBoxTrigger : MonoBehaviour
{
    string objName;
    // Start is called before the first frame update
    void Start()
    {
        objName = gameObject.name;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void DamageHealth(int Dem)
	{
        //몬스터 공용 HitBoxTrigger 스크립트
        //플레이어 공격 함수에서 각 몬스터 데미지 함수를 쉽게 불러오기 위함.
        //오브젝트 name으로 구분. 플레이어 -> 게임 컴포넌트 Type = 스크립트 이름으로 호출. 스크립트 -> 게임 오브젝트 = 프리팹 트리거 오브젝트 이름으로 호출.
        switch (objName)
        {
            case "HitBoxTriggerDye":
                gameObject.GetComponentInParent<DyeScript>().DamageHealth(Dem);
                break;
        
            case "HitBoxTriggerEarth":
                gameObject.GetComponentInParent<EarthScript>().DamageHealth(Dem);
                break;        
            default:
                break;       
	    }
    }
    public int GetMonsterScore()
    {
        switch (objName)
        {
            case "HitBoxTriggerDye":
                return GetComponentInParent<DyeScript>().GetScore();
        
            case "HitBoxTriggerEarth":
                return GetComponentInParent<EarthScript>().GetScore();
        
            default:
                return 0;
       
	    }
    }
}
