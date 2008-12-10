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
	Posture InterpPosture;

	// Add your code here

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