#include "simulation/Elements.h"

//#TPT-Directive ElementClass Element_LSNS PT_LSNS 185
Element_LSNS::Element_LSNS()
{
	Identifier = "DEFAULT_PT_LSNS";
	Name = "LSNS";
	Colour = PIXPACK(0x336699);
	MenuVisible = 1;
	MenuSection = SC_SENSOR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f	* CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	DefaultProperties.temp = 4.0f + 273.15f;
	DefaultProperties.tmp2 = 2;
	HeatConduct = 0;
	Description = "Life sensor, creates a spark when there's a nearby particle with a life higher than its temperature.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_LSNS::update;
}

//#TPT-Directive ElementHeader Element_LSNS static int update(UPDATE_FUNC_ARGS)
int Element_LSNS::update(UPDATE_FUNC_ARGS)
{
	int rd = parts[i].tmp2;
	if (rd > 25) parts[i].tmp2 = rd = 25;
	if (parts[i].life)
	{
		parts[i].life = 0;
		for (int rx = -2; rx <= 2; rx++)
			for (int ry = -2; ry <= 2; ry++)
				if (BOUNDS_CHECK && (rx || ry))
				{
					int r = pmap[y + ry][x + rx];
					if (!r)
						continue;
					int rt = TYP(r); r = ID(r);
					if (!sim->parts_avg_elec(i, r))
					{
						if ((sim->elements[rt].Properties&(PROP_CONDUCTS|PROP_INSULATED)) == PROP_CONDUCTS && parts[r].life == 0)
						{
							parts[r].life = 4;
							parts[r].ctype = rt;
							sim->part_change_type(r, x + rx, y + ry, PT_SPRK);
						}
					}

				}
	}
	bool doSerialization = false;
	bool doDeserialization = false;
	int life = 0;
	for (int rx = -rd; rx < rd + 1; rx++)
		for (int ry = -rd; ry < rd + 1; ry++)
			if (x + rx >= 0 && y + ry >= 0 && x + rx < XRES && y + ry < YRES && (rx || ry))
			{
				int r = pmap[y + ry][x + rx];
				if (!r)
					r = sim->photons[y + ry][x + rx];
				if (!r)
					continue;

				switch (parts[i].tmp)
				{
				case 1:
					// .life serialization into FILT
					if (TYP(r) != PT_LSNS && TYP(r) != PT_FILT && parts[ID(r)].life >= 0)
					{
						doSerialization = true;
						life = parts[ID(r)].life;
					}
					break;
				case 3:
					// .life deserialization
					if (TYP(r) == PT_FILT)
					{
						doDeserialization = true;
						life = parts[ID(r)].ctype;
					}
					break;
				case 2:
					// Invert mode
					if (TYP(r) != PT_METL && parts[ID(r)].life <= parts[i].temp - 273.15)
						parts[i].life = 1;
					break;
				default:
					// Normal mode
					if (TYP(r) != PT_METL && parts[ID(r)].life > parts[i].temp - 273.15)
						parts[i].life = 1;
					break;
				}
			}

	for (int rx = -1; rx <= 1; rx++)
		for (int ry = -1; ry <= 1; ry++)
			if (BOUNDS_CHECK && (rx || ry))
			{
				int r = pmap[y + ry][x + rx];
				if (!r)
					continue;
				int nx = x + rx;
				int ny = y + ry;
				// .life serialization.
				if (doSerialization)
				{
					while (TYP(r) == PT_FILT)
					{
						parts[ID(r)].ctype = 0x10000000 + life;
						nx += rx;
						ny += ry;
						if (nx < 0 || ny < 0 || nx >= XRES || ny >= YRES)
							break;
						r = pmap[ny][nx];
					}
				}
				// .life deserialization.
				else if (doDeserialization)
				{
					if (TYP(r) != PT_FILT)
						parts[ID(r)].life = life - 0x10000000;
				}
			}

	return 0;
}

Element_LSNS::~Element_LSNS() {}
