#include "posture.h"

/************************ Posture class functions **********************************/

Posture::Posture(const Posture& rhs)
{
	root_pos = rhs.root_pos;

	int i;
	for(i=0;i<MAX_BONES_IN_ASF_FILE;i++)
		bone_rotation[i] = rhs.bone_rotation[i];
	for(i=0;i<MAX_BONES_IN_ASF_FILE;i++)
		bone_translation[i] = rhs.bone_translation[i];
	for(i=0;i<MAX_BONES_IN_ASF_FILE;i++)
		bone_length[i] = rhs.bone_length[i];
}

//Output Posture  = (1-t)*a + t*b
Posture 
LinearInterpolate(float t, Posture const& a, Posture const& b )
{
	Posture InterpPosture;

	//Iterpolate root position
	InterpPosture.root_pos = Vector3::interpolate(a.root_pos, b.root_pos, t);

	//Interpolate bones rotations
	for (int i = 0; i < MAX_BONES_IN_ASF_FILE; i++)
		InterpPosture.bone_rotation[i] = Vector3::interpolate(a.bone_rotation[i], b.bone_rotation[i], t);

	return InterpPosture;
}

/************************ Posture class functions **********************************/

//Output Posture  = (1-t)*a + t*b
Posture Slerp(float t, Posture& a, Posture& b )
{
	int i;
	Posture InterpPosture;
	Quaternion q1, q2;

	InterpPosture.root_pos = Vector3::interpolate(a.root_pos, b.root_pos, (double)t);

	for (i=0; i<Posture::NUM_BONES; i++)
	{
		q1.FromEulerAngle_1(a.bone_rotation[i]);
		q2.FromEulerAngle_1(b.bone_rotation[i]);
		InterpPosture.bone_rotation[i] = Quaternion::Slerp1(q1, q2, (double)t).ToEulerAngle();

		//InterpPosture.bone_rotation[i] = Vector3::interpolate(a.bone_rotation[i], b.bone_rotation[i], t);
	}
	return InterpPosture;
}

void Posture::Rotate(float theta)
{
	Matrix4 m1, m2, mr;
	m1.FromEulerAngle(bone_rotation[0]*deg2rad);
	m2 = Matrix4::Yrotate(theta*deg2rad);
	mr = m2 * m1;
	bone_rotation[0] = mr.ToEulerAngle(mr)*rad2deg;
}

void Posture::computeJointVelocities(Posture& p, bool Forward)
{
	int i;
	Quaternion q1, q2, q2Inverse;

	if(Forward)	//	p is right after this posture
	{
		for (i=1; i < NUM_BONES; i++)
		{
			q1.FromEulerAngle_1(bone_rotation[i]);
			q2.FromEulerAngle_1(p.bone_rotation[i]);
			joint_velocity[i] = q2.Inverse() * q1;
		}
	}
	else
	{
		for (i=1; i < NUM_BONES; i++)
		{
			q2.FromEulerAngle_1(bone_rotation[i]);
			q1.FromEulerAngle_1(p.bone_rotation[i]);
			joint_velocity[i] = q2.Inverse() * q1;
		}
	}
}

/************************ static functions *****************************/
double Posture::compareJointAngles(Posture& p1, Posture& p2)
{
	int i;
	Quaternion q1, q2;
	double poseDiff = 0.0;

	for (i = 1; i < NUM_BONES; i++)
	{
		q1.FromEulerAngle_1(p1.bone_rotation[i]);
		q2.FromEulerAngle_1(p2.bone_rotation[i]);
		poseDiff += getJointWeight(i) * (q2.Inverse() * q1).Log().Norm();
	}

	return poseDiff;
}

double Posture::compareJointVelocities(Posture& p1, Posture& p2)
{
	int i;
	double velDiff = 0.0f;
	Quaternion *q1, *q2;


	for (i=0; i<NUM_BONES; i++)
	{
		q1 = &p1.joint_velocity[i];
		q2 = &p2.joint_velocity[i];
		velDiff += getJointWeight(i) * (q2->Inverse() * (*q1)).Log().Norm();	
	}
	return velDiff;
}


double Posture::getJointWeight(int joint)
{
	switch(joint)
	{
	//	less impact on visible difference
	case Root:	//	can be aligned
	case lhipjoint:
	case lfoot:
	case ltoes:
	case rhipjoint:
	case rfoot:
	case rtoes:
	case lowerneck:
	case upperneck:
	case head:
	case lwrist:
	case lhand:
	case lfingers:
	case lthumb:
	case rwrist:
	case rhand:
	case rfingers:
	case rthumb:
		return 0.0f;
		break;
	// more impact
	case lfemur:
	case ltibia:
	case rfemur:
	case rtibia:
	case lowerback:
	case upperback:
	case thorax:
	case lclavicle:
	case lhumerus:
	case lradius:
	case rclavicle:
	case rhumerus:
	case rradius:
		return 1.0f;
		break;
	}
	return 0.0f;
}