#include "../tomb4/pch.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "draw.h"
#include "lara.h"
#include "objects.h"
#include "control.h"
#include "../specific/file.h"
#include "../specific/platform.h"

SPHERE Slist[34];
char GotLaraSpheres;

static SPHERE LaraSpheres[15];
static long NumLaraSpheres;

long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* meshp;
	short* frame;
	short* rot;
	short* extra_rot;
	long x, y, z, poppush;

	if (!item)
		return 0;

	if (WorldSpace & 1)
	{
		x = item->pos.x_pos;
		y = item->pos.y_pos;
		z = item->pos.z_pos;
		phd_PushUnitMatrix();
		phd_SetTrans(0, 0, 0);
	}
	else
	{
		z = 0;
		y = 0;
		x = 0;
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	}

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	frame = GetBestFrame(item);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);
	obj = &objects[item->object_number];
	meshpp = &meshes[obj->mesh_index];
	meshp = *meshpp;
	meshpp += 2;
	bone = &bones[obj->bone_index];

	phd_PushMatrix();

	if (!(WorldSpace & 2))
		phd_TranslateRel(meshp[0], meshp[1], meshp[2]);

	ptr->x = x + (long)mMXPtr[M03];
	ptr->y = y + (long)mMXPtr[M13];
	ptr->z = z + (long)mMXPtr[M23];
	ptr->r = meshp[3];
	ptr++;
	phd_PopMatrix();

	extra_rot = (short*)item->data;

	for (int i = 0; i < obj->nmeshes - 1; i++)
	{
		poppush = *bone++;

		if (poppush & POP_BONE_FLAG)
			phd_PopMatrix();

		if (poppush & PUSH_BONE_FLAG)
			phd_PushMatrix();

		phd_TranslateRel(bone[0], bone[1], bone[2]);
		gar_RotYXZsuperpack(&rot, 0);

		if (poppush & (X_ROTATION_FLAG | Y_ROTATION_FLAG | Z_ROTATION_FLAG) && extra_rot)
		{
			if (poppush & Y_ROTATION_FLAG)
				phd_RotY(*extra_rot++);

			if (poppush & X_ROTATION_FLAG)
				phd_RotX(*extra_rot++);

			if (poppush & Z_ROTATION_FLAG)
				phd_RotZ(*extra_rot++);
		}

		meshp = *meshpp;
		phd_PushMatrix();

		if (!(WorldSpace & 2))
			phd_TranslateRel(meshp[0], meshp[1], meshp[2]);

		ptr->x = x + (long)mMXPtr[M03];
		ptr->y = y + (long)mMXPtr[M13];
		ptr->z = z + (long)mMXPtr[M23];
		ptr->r = meshp[3];
		ptr++;
		phd_PopMatrix();

		bone += 3;
		meshpp += 2;
	}

	phd_PopMatrix();
	return obj->nmeshes;
}

long TestCollision(ITEM_INFO* item, ITEM_INFO* l)
{
	SPHERE* itemSpheres;
	SPHERE* laraSpheres;
	PHD_VECTOR ip;
	PHD_VECTOR lp;
	ulong touch_bits;
	long nItemSpheres, nLaraSpheres, ir, lr;

	touch_bits = 0;
	nItemSpheres = GetSpheres(item, Slist, 1);

	if (l != lara_item)
		GotLaraSpheres = 0;

	if (GotLaraSpheres)
		nLaraSpheres = NumLaraSpheres;
	else
	{
		nLaraSpheres = GetSpheres(l, LaraSpheres, 1);
		NumLaraSpheres = nLaraSpheres;

		if (l == lara_item)
			GotLaraSpheres = 1;
	}

	for (int i = 0; i < nItemSpheres; i++)
	{
		itemSpheres = &Slist[i];
		ir = itemSpheres->r;

		if (ir > 0)
		{
			ip.x = itemSpheres->x;
			ip.y = itemSpheres->y;
			ip.z = itemSpheres->z;

			for (int j = 0; j < nLaraSpheres; j++)
			{
				laraSpheres = &LaraSpheres[j];
				lr = laraSpheres->r;

				if (lr > 0)
				{
					lp.x = ip.x - laraSpheres->x;
					lp.y = ip.y - laraSpheres->y;
					lp.z = ip.z - laraSpheres->z;
					lr += ir;

					if (SQUARE(lp.x) + SQUARE(lp.y) + SQUARE(lp.z) < SQUARE(lr))
					{
						touch_bits |= 1 << i;
						break;
					}
				}
			}
		}
	}

	item->touch_bits = touch_bits;
	return touch_bits;
}

void InitInterpolate2(long frac, long rate)
{
	IM_frac = frac;
	IM_rate = rate;
	mIMptr = &mIMstack[384];
	memcpy(mIMptr, mMXPtr, 48);
}

void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, long joint)
{
	OBJECT_INFO* obj;
	float* mMx;
	float* mIMx;
	long* iMx;
	long* bone;
	short* frm[2];
	short* extra_rotation;
	short* rot;
	short* rot2;
	long frac, rate, poppush;

	// T4Plus: Animation safety check
	if (item->anim_number < 0 || item->anim_number >= num_anims) {
		platform_fatal_error("GetJointAbsPosition: invalid animation data!");
		return;
	}

	mMx = mMXPtr;
	mIMx = mIMptr;
	iMx = phd_mxptr;
	obj = &objects[item->object_number];
	frac = GetFrames(item, frm, &rate);

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	extra_rotation = (short*)item->data;

	if (!extra_rotation)
		extra_rotation = no_rotation;

	bone = &bones[obj->bone_index];

	if (frac)
	{
		InitInterpolate2(frac, rate);
		rot = frm[0] + 9;
		rot2 = frm[1] + 9;
		phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		for (int i = 0; i < joint; i++)
		{
			poppush = *bone;

			if (poppush & POP_BONE_FLAG)
				phd_PopMatrix_I();

			if (poppush & PUSH_BONE_FLAG)
				phd_PushMatrix_I();

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (poppush & (X_ROTATION_FLAG | Y_ROTATION_FLAG | Z_ROTATION_FLAG))
			{
				if (poppush & Y_ROTATION_FLAG)
					phd_RotY_I(*extra_rotation++);

				if (poppush & X_ROTATION_FLAG)
					phd_RotX_I(*extra_rotation++);

				if (poppush & Z_ROTATION_FLAG)
					phd_RotZ_I(*extra_rotation++);
			}

			bone += 4;
		}

		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		mInterpolateMatrix();
	}
	else
	{
		phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
		rot = frm[0] + 9;
		gar_RotYXZsuperpack(&rot, 0);

		for (int i = 0; i < joint; i++)
		{
			poppush = *bone;

			if (poppush & POP_BONE_FLAG)
				phd_PopMatrix();

			if (poppush & PUSH_BONE_FLAG)
				phd_PushMatrix();

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot, 0);

			if (poppush & (X_ROTATION_FLAG | Y_ROTATION_FLAG | Z_ROTATION_FLAG))
			{
				if (poppush & Y_ROTATION_FLAG)
					phd_RotY(*extra_rotation++);

				if (poppush & X_ROTATION_FLAG)
					phd_RotX(*extra_rotation++);

				if (poppush & Z_ROTATION_FLAG)
					phd_RotZ(*extra_rotation++);
			}

			bone += 4;
		}

		phd_TranslateRel(pos->x, pos->y, pos->z);
	}

	pos->x = item->pos.x_pos + (long)mMXPtr[M03];
	pos->y = item->pos.y_pos + (long)mMXPtr[M13];
	pos->z = item->pos.z_pos + (long)mMXPtr[M23];
	mMXPtr = mMx;
	mIMptr = mIMx;
	phd_mxptr = iMx;
}
