//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent, TargetData* pTarget)
{
	SteeringOutput steering{};
	const Elite::Vector2 agentPos{ pAgent->GetPosition() };
	const Elite::Vector2 agentToTargetVec = pTarget->Position - agentPos;

	steering.LinearVelocity = agentToTargetVec.GetNormalized() * pAgent->GetMaxLinearSpeed();

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(agentPos, steering.LinearVelocity, 5, { 0.f, 1.f, 0.f, 0.5f }, 0.4f);

	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	///Generate random point in a circle in front of agent
	const Elite::Vector2 circleOffset{ pAgent->GetLinearVelocity().GetNormalized() * m_Offset };

	//Generate angle in RAD
	m_WanderAngle += randomFloat(-m_MaxAngleChange / 2.f, m_MaxAngleChange / 2.f);
	//Convert to vector (normalized)
	const Elite::Vector2 wanderDir{ cosf(m_WanderAngle), sinf(m_WanderAngle) };

	m_Target = TargetData{ pAgent->GetPosition() + circleOffset + wanderDir * m_Radius };

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawPoint(m_Target.Position, 5, { 1.f, 0.f, 0.f, 0.5f }, 0.3f);
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition() + circleOffset, m_Radius, { 0.1f, 0.1f, 1.f, 0.5f }, 0.4f);
	}
	
	//Seek point
	return Seek::CalculateSteering(deltaT, pAgent);
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent, TargetData* pTarget)
{
	SteeringOutput steering{};
	const Elite::Vector2 agentPos{ pAgent->GetPosition() };
	const Elite::Vector2 targetToAgentvec = agentPos - pTarget->Position;

	steering.LinearVelocity = targetToAgentvec.GetNormalized() * pAgent->GetMaxLinearSpeed();

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(agentPos, steering.LinearVelocity, 5, { 0.f, 1.f, 0.f, 0.5f }, 0.4f);

	return steering;
}

//ARRIVE
//******
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	const Elite::Vector2 agentPos{ pAgent->GetPosition() };
	const Elite::Vector2 agentToTargetVec = m_Target.Position - agentPos;
	const float agentToTargetDistanceSquared{ agentToTargetVec.MagnitudeSquared() };
	
	if (agentToTargetDistanceSquared <= m_ArriveRadius * m_ArriveRadius) //stop
		steering.LinearVelocity = Elite::ZeroVector2;
	else if (agentToTargetDistanceSquared <= m_SlowRadius * m_SlowRadius) //slow down
	{
		float speedStrength{ sqrtf(agentToTargetDistanceSquared) / m_SlowRadius };
		steering.LinearVelocity = agentToTargetVec.GetNormalized() * pAgent->GetMaxLinearSpeed() * speedStrength;
	}
	else //max speed
		steering.LinearVelocity = agentToTargetVec.GetNormalized() * pAgent->GetMaxLinearSpeed();

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(agentPos, steering.LinearVelocity, 5, { 0.f, 1.f, 0.f, 0.5f }, 0.4f);

	return steering;
}

//FACE
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	const float pi{ float(M_PI) };
	const Elite::Vector2 agentPos{ pAgent->GetPosition() };
	const Elite::Vector2 agentToTargetVecNorm{ (m_Target.Position - agentPos).GetNormalized() };
	const float currAngle{ pAgent->GetOrientation() };
	const float desiredAngle{ atan2(agentToTargetVecNorm.y, agentToTargetVecNorm.x) + (pi / 2.f) };

	//Rotate
	pAgent->SetAutoOrient(false);

	float rotation{ desiredAngle - currAngle };
	while (rotation > pi) rotation -= (2.f * pi);
	while (rotation < -pi) rotation += (2.f * pi);

	const float speed = pAgent->GetMaxAngularSpeed();
	steering.AngularVelocity = Clamp(ToDegrees(rotation), -speed, speed);

	return steering;
}

//EVADE (base> FLEE)
//*****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_TempTarget = m_Target;
	
	float distToTarget = Distance(pAgent->GetPosition(), m_TempTarget.Position);

	if(distToTarget > m_EvadeRadius)
	{	//Too far to evade
		SteeringOutput steering;
		steering.IsValid = false;
		return steering;
	}

	//Scale future position of target down if you are too close to the target
	Elite::Vector2 futurePosOffset{ m_TempTarget.LinearVelocity };

	if(distToTarget * distToTarget < futurePosOffset.MagnitudeSquared())
	{	//Agent to target < futurePos to target, put future position closer to target
		futurePosOffset.Normalize();
		futurePosOffset *= (distToTarget * 0.95f); //futurePos is now closer to the target than agent
	}

	//Set future position of target
	m_TempTarget.Position += futurePosOffset;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawPoint(m_TempTarget.Position, 5, { 0.1f, 0.1f, 1.f, 0.2f }, 0.4f);
		DEBUGRENDERER2D->DrawDirection(m_TempTarget.Position - futurePosOffset, futurePosOffset, futurePosOffset.Magnitude(), { 0.1f, 0.1f, 1.f, 0.2f }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, { 1.f, 0.f, 0.f, 1.f }, 0.4f);
	}
	
	//Seek point
	return Flee::CalculateSteering(deltaT, pAgent, &m_TempTarget);
}

//PURSUIT (base> SEEK)
//*******
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_TempTarget = m_Target;
	
	float distToTarget = Distance(pAgent->GetPosition(), m_TempTarget.Position);

	//Scale future position of target down if you are too close to the target
	Elite::Vector2 futurePosOffset{ m_TempTarget.LinearVelocity };

	if (distToTarget * distToTarget < futurePosOffset.MagnitudeSquared())
	{	//Agent to target < futurePos to target, put future position closer to target
		futurePosOffset.Normalize();
		futurePosOffset *= (distToTarget * 0.95f); //futurePos is now closer to the target than agent
	}

	//Set future position of target
	m_TempTarget.Position += futurePosOffset;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawPoint(m_TempTarget.Position, 5, { 0.1f, 0.1f, 1.f, 0.2f }, 0.4f);
		DEBUGRENDERER2D->DrawDirection(m_TempTarget.Position - futurePosOffset, futurePosOffset, futurePosOffset.Magnitude(), { 0.1f, 0.1f, 1.f, 0.2f }, 0.4f);
	}
	
	//Seek point
	return Seek::CalculateSteering(deltaT, pAgent, &m_TempTarget);
}