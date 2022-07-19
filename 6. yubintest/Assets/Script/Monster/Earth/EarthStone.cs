using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EarthStone : MonoBehaviour
{
    public float waveSpeed ;
    public static Vector3 playerPositionStatic;
    
    Vector3 playerPosition;
    // Start is called before the first frame update
    void Start()
    {
        playerPosition = playerPositionStatic - gameObject.transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        transform.Translate(playerPosition * Time.deltaTime * waveSpeed);     
        Destroy(gameObject,5);        
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.tag == "PlayerIdleCollider") 
		{			
            other.gameObject.GetComponent<IdleColl>().Damage(1);
            Destroy(gameObject);
		}
        if(other.gameObject.tag == "Platform" && !other.isTrigger) 
            Destroy(gameObject);
        if(other.gameObject.tag == "Wall" && !other.isTrigger) 
            Destroy(gameObject);
    }

    public static void SetPlayerPosition(Vector3 position)
    {
        playerPositionStatic = position;
    }
}
