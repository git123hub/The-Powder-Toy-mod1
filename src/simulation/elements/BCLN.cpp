#include "simulation/Elements.h"
//#TPT-Directive ElementClass Element_BCLN PT_BCLN 93
Element_BCLN::Element_BCLN()
{
	Identifier = "DEFAULT_PT_BCLN";
	Name = "BCLN";
	Colour = PIXPACK(0xFFD040);
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.97f;
	Loss = 0.50f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 12;

	Weight = 100;

	HeatConduct = 251;
	Description = "Breakable Clone.";

	Properties = TYPE_SOLID|PROP_LIFE_DEC|PROP_LIFE_KILL_DEC|PROP_DRAWONCTYPE|PROP_NOCTYPEDRAW | PROP_TRANSPARENT;
	Properties2 = PROP_CLONE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_BCLN::update;
}

#define ADVECTION 0.1f

//#TPT-Directive ElementHeader Element_BCLN static int update(UPDATE_FUNC_ARGS)
int Element_BCLN::update(UPDATE_FUNC_ARGS)
{
	if (!parts[i].life && sim->pv[y/CELL][x/CELL]> sim->sim_max_pressure)
		parts[i].life = rand()%40+80;
	if (parts[i].life)
	{
		parts[i].vx += ADVECTION*sim->vx[y/CELL][x/CELL];
		parts[i].vy += ADVECTION*sim->vy[y/CELL][x/CELL];
	}
	int ctype1 = parts[i].ctype;
	if (ctype1 <= 0 || ctype1 >= PT_NUM || !sim->elements[ctype1].Enabled || (ctype1==PT_LIFE && (parts[i].tmp<0 || parts[i].tmp>=NGOL)))
	{
		int r, rx, ry, rt;
		for (rx=-1; rx<2; rx++)
			for (ry=-1; ry<2; ry++)
				if (BOUNDS_CHECK)
				{
					r = sim->photons[y+ry][x+rx];
					if (!r)
						r = pmap[y+ry][x+rx];
					if (!r)
						continue;
					rt = TYP(r);
					if (!(sim->elements[rt].Properties2 & PROP_CLONE)
						&& rt!=PT_STKM && rt!=PT_STKM2 && rt!=PT_E195 && rt<PT_NUM)
					{
						parts[i].ctype = rt;
						if (rt==PT_LIFE || rt==PT_LAVA)
							parts[i].tmp = partsi(r).ctype;
					}
				}
	}
	else {
		if (ctype1 == PT_LIFE) sim->create_part(-1, x+rand()%3-1, y+rand()%3-1, PT_LIFE, parts[i].tmp);
		else if (ctype1 != PT_LIGH || (rand()%30)==0)
		{
			int np = sim->create_part(-1, x+rand()%3-1, y+rand()%3-1, TYP(ctype1));
			if (np>=0)
			{
				if ((ctype1 == PT_LAVA && parts[i].tmp>0 && parts[i].tmp<PT_NUM && sim->elements[parts[i].tmp].HighTemperatureTransition==PT_LAVA) || ctype1 == PT_E195)
					parts[np].ctype = parts[i].tmp;
			}
		}
	}
	return 0;
}


Element_BCLN::~Element_BCLN() {}
