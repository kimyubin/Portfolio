// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<vector>
#include<functional>
#include<memory>

using namespace std;

/*
 * 입력으로 필드정보와 프리랜서 정보를 주면, 현재 상태에 맞는 동작을 수행
 * 인스턴스 생성 없이 스태틱 함수들로만 구성할 것(클래스라기보단 namespace의 역할)
 * 역할이 없다 싶으면 제거하고 FSM에 통합할것임
 */
class ENOCH_API EnochFreeLancerAct
{
public:
	EnochFreeLancerAct() = delete;

	static void Init(class EnochSimulator *simulatorIn);
	static void Release() { functionMap = nullptr; simulator = nullptr; }
	//현재 동작을 수행. bool이 false인 경우, 동작이 바뀌지 않아야 하므로 FSM을 실행하지 말라는 의미
	static bool Act(class EnochFreeLancerData *freeLancer);

private :
	static unique_ptr<vector<function<bool(EnochFreeLancerData*)>>> functionMap;
	static class EnochSimulator* simulator;
};