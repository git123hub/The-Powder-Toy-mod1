#include <iostream>
#include "graphics/Graphics.h"
#include "Tool.h"
#include "GameModel.h"
#include "gui/interface/Colour.h"

VideoBuffer * SampleTool::GetIcon(int toolID, int width, int height)
{
	VideoBuffer * newTexture = new VideoBuffer(width, height);
	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			pixel pc =  x==0||x==width-1||y==0||y==height-1 ? PIXPACK(0xA0A0A0) : PIXPACK(0x000000);
			newTexture->SetPixel(x, y, PIXR(pc), PIXG(pc), PIXB(pc), 255);
		}
	}
	newTexture->SetCharacter((width/2)-5, (height/2)-5, 0xE6, 255, 255, 255, 255);
	newTexture->BlendPixel(10, 9, 100, 180, 255, 255);
	newTexture->BlendPixel(11, 8, 100, 180, 255, 255);
	newTexture->BlendPixel(12, 7, 100, 180, 255, 255);
	return newTexture;
}

void SampleTool::Draw(Simulation * sim, Brush * brush, ui::Point position)
{
	if(gameModel->GetColourSelectorVisibility())
	{
		pixel colour = gameModel->GetRenderer()->sampleColor;
		gameModel->SetColourSelectorColour(ui::Colour(PIXR(colour), PIXG(colour), PIXB(colour), 255));
	}
	else
	{
		int particleType = 0;
		int particleCtype = 0;
		if (sim->photons[position.Y][position.X])
		{
			particleType = sim->parts[ID(sim->photons[position.Y][position.X])].type;
			particleCtype = sim->parts[ID(sim->photons[position.Y][position.X])].ctype;
		}
		else if (sim->pmap[position.Y][position.X])
		{
			particleType = sim->parts[ID(sim->pmap[position.Y][position.X])].type;
			particleCtype = sim->parts[ID(sim->pmap[position.Y][position.X])].ctype;
		}

		if(particleType)
		{
			if(particleType == PT_LIFE)
			{
				Menu * lifeMenu = gameModel->GetMenuList()[SC_LIFE];
				std::vector<Tool*> elementTools = lifeMenu->GetToolList();

				for(std::vector<Tool*>::iterator iter = elementTools.begin(), end = elementTools.end(); iter != end; ++iter)
				{
					Tool * elementTool = *iter;
					if(elementTool)
					{
						int ToolID1 = elementTool->GetToolID();
						if (ID(ToolID1) == particleCtype && TYP(ToolID1) == PT_LIFE)
							gameModel->SetActiveTool(0, elementTool);
					}
				}
			}
			else
			{
				if (particleType == ELEM_MULTIPP)
				{
					int i = ID(sim->pmap[position.Y][position.X]);
					int particleLife = sim->parts[i].life;
					int menu_section_1 = SC_SPECIAL;
					if (particleLife == 33 || particleLife == 37)
					{
						particleType |= PMAPID(particleLife);
						menu_section_1 = (particleLife == 37 ? SC_LIFE : SC_ELEC);
					}

					Menu * elemMenu = gameModel->GetMenuList()[menu_section_1];
					std::vector<Tool*> elementTools = elemMenu->GetToolList();

					for(std::vector<Tool*>::iterator iter = elementTools.begin(), end = elementTools.end(); iter != end; ++iter)
					{
						Tool * elementTool = *iter;
						if(elementTool)
						{
							int ToolID1 = elementTool->GetToolID();
							if (ToolID1 == particleType)
								gameModel->SetActiveTool(0, elementTool);
						}
					}
					return;
				}
				Tool * elementTool = gameModel->GetElementTool(particleType);
				if(elementTool)
					gameModel->SetActiveTool(0, elementTool);
			}
		}
	}
}
