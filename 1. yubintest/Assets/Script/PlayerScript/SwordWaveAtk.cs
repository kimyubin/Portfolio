using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SwordWaveAtk : MonoBehaviour
{
    
	public GameObject swordWaveObject;
    public float SwordWaveDelay;

	bool swordWaveState; //true = 공격 중 = 공격불가능 상태, false = 공격 아님 = 공격 가능상태
	
    // Start is called before the first frame update
    void Start()
    {
       swordWaveState = false;
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetButtonDown("Fire") && !swordWaveState)
            SwordWaveFire();

    }
    void SwordWaveFire()
	{
		StartCoroutine(SwordWaveControl());
        Instantiate(swordWaveObject,gameObject.transform.position,gameObject.transform.rotation);
	}

	IEnumerator SwordWaveControl()
	{
		swordWaveState = true;
		gameObject.transform.parent.GetComponent<Player>().animator.SetTrigger("animSwordWaveTrigger");
		yield return new WaitForSeconds(SwordWaveDelay);
  		swordWaveState = false;		
	}
    public bool GetWaveState()
    {
        return swordWaveState;
    }
}
