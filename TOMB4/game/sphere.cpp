#include "../tomb4/pch.h"
#include "sphere.h"
#include "../specific/3dmath.h"
#include "draw.h"

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
		phd_mxptr[M03] = 0;
		phd_mxptr[M13] = 0;
		phd_mxptr[M23] = 0;
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

	ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
	ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
	ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
	ptr->r = meshp[3];
	ptr++;
	phd_PopMatrix();

	extra_rot = (short*)item->data;

	for (int i = 0; i < obj->nmeshes - 1; i++)
	{
		poppush = *bone++;

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[0], bone[1], bone[2]);
		gar_RotYXZsuperpack(&rot, 0);

		if (poppush & 0x1C && extra_rot)
		{
			if (poppush & 8)
				phd_RotY(*extra_rot++);

			if (poppush & 4)
				phd_RotX(*extra_rot++);

			if (poppush & 0x10)
				phd_RotZ(*extra_rot++);
		}

		meshp = *meshpp;
		phd_PushMatrix();

		if (!(WorldSpace & 2))
			phd_TranslateRel(meshp[0], meshp[1], meshp[2]);

		ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
		ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
		ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
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
	IMptr = &IMstack[384];
	memcpy(IMptr, phd_mxptr, 48);
}

void inject_sphere(bool replace)
{
	INJECT(0x0045FC90, GetSpheres, replace);
	INJECT(0x0045FB10, TestCollision, replace);
	INJECT(0x00460270, InitInterpolate2, replace);
}
