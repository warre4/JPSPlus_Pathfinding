/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
class SteeringAgent;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As() { return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

//////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	inline SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override { return CalculateSteering(deltaT, pAgent, &m_Target); }
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent, TargetData* pTarget);
};

//////////////////////////
//WANDER
//******
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	inline void SetWanderOffset(float offset) { m_Offset = offset; }
	inline void SetWanderRadius(float radius) { m_Radius = radius; }
	inline void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }
	
protected:
	float m_Offset{ 6.f };
	float m_Radius{ 4.f };
	float m_MaxAngleChange{ ToRadians(45.f) };
	float m_WanderAngle{ 0.f };
};

//////////////////////////
//FLEE
//****
class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Flee Behaviour
	inline SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override { return CalculateSteering(deltaT, pAgent, &m_Target); }
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent, TargetData* pTarget);
};

//////////////////////////
//ARRIVE
//****
class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	//Arrive Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetSlowRadius(float slowRadius) { m_SlowRadius = slowRadius; }
	void SetArriveRadius(float arriveRadius) { m_ArriveRadius = arriveRadius; }
	
protected:
	float m_SlowRadius{ 8.f };
	float m_ArriveRadius{ 2.f };
};

//////////////////////////
//FACE
//****
class Face : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() = default;

	//Face Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

//////////////////////////
//EVADE
//*****
class Evade : public Flee
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	//Evade Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	inline void SetEvadeRadius(float evadeRadius) { m_EvadeRadius = evadeRadius; }
	
protected:
	float m_EvadeRadius{ 15.f };

	TargetData m_TempTarget{};
};

//////////////////////////
//PURSUIT
//*******
class Pursuit : public Seek
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	//Pursuit Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

protected:
	TargetData m_TempTarget{};
};
#endif