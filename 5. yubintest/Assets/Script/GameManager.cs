using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour {

	public GameObject player;
	Vector3 StartingPos;
	Quaternion StratingRotate;
	bool isStarted = false;
	static bool isEnded = false;

	static int stageLevel = 0;

	void Awake()
	{
		Time.timeScale = 0f;
	}

	// Use this for initialization
	void Start () 
	{
		Physics2D.IgnoreLayerCollision(8, 9, true);

		StartingPos = GameObject.FindGameObjectWithTag("Start").transform.position;
		StratingRotate = GameObject.FindGameObjectWithTag("Start").transform.rotation;

		if(stageLevel >0 )
			startGame();
	}
	
	void OnGUI()
	{

		GUILayout.BeginArea(new Rect(0,0,Screen.width,Screen.height));
		GUILayout.BeginHorizontal();
		GUILayout.FlexibleSpace();
		GUILayout.BeginVertical();
		GUILayout.FlexibleSpace();

		if(stageLevel < 3)
			GUILayout.Label("Stage "+ (stageLevel));
		else
			GUILayout.Label("Stage End");
	
		GUILayout.Space(5);
		GUILayout.EndVertical();
		GUILayout.FlexibleSpace();
		GUILayout.EndHorizontal();
		GUILayout.EndArea();
		
		
		if(!isStarted && stageLevel ==0)
		{
			GUILayout.BeginArea(new Rect(0,0,Screen.width,Screen.height));
			GUILayout.BeginHorizontal();
			GUILayout.FlexibleSpace();
			GUILayout.BeginVertical();
			GUILayout.FlexibleSpace();

			GUILayout.Label("R U Ready?");
			if(GUILayout.Button("Start!"))
			{
				isStarted = true;
				startGame();
			}
			
			GUILayout.Space(100);

			GUILayout.FlexibleSpace();
			GUILayout.EndVertical();

			GUILayout.FlexibleSpace();
			GUILayout.EndHorizontal();

			GUILayout.EndArea();
		}
		else if(isEnded && stageLevel == 2)
		{
			GUILayout.BeginArea(new Rect(0,0,Screen.width,Screen.height));
			GUILayout.BeginHorizontal();
			GUILayout.FlexibleSpace();
			GUILayout.BeginVertical();
			GUILayout.FlexibleSpace();

			GUILayout.Label("Thank you for play");
			if(GUILayout.Button("restrat?"))
			{
				isEnded = false;
				stageLevel =0;
				ScoreManager.setScore(0);
				SceneManager.LoadScene(0, LoadSceneMode.Single);
				
			}
			GUILayout.Space(100);

			GUILayout.FlexibleSpace();
			GUILayout.EndVertical();

			GUILayout.FlexibleSpace();
			GUILayout.EndHorizontal();

			GUILayout.EndArea();
		}
	}

	void startGame()
	{
		Time.timeScale = 1f;
		
		StartingPos = new Vector3(StartingPos.x,StartingPos.y+0.5f, StartingPos.z);
		Instantiate(player,StartingPos,StratingRotate);
	}

	public static void EndGame()//isEnded가 static이므로 static
	{
		Time.timeScale = 0f;

		stageLevel++;

		if(stageLevel==2)
		{
			isEnded = true;
		}
		else
		{
			SceneManager.LoadScene(stageLevel);

		}
	}

	public static void RestartStage()
	{
		Time.timeScale = 0f;
	}

	// Update is called once per frame
	void Update () {
		
	}
}
