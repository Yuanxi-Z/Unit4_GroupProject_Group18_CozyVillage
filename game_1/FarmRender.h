#ifndef FARM_RENDER_H
#define FARM_RENDER_H

#include "FarmState.h"

const char *FarmRender_GetModeName(FarmMode mode);
const char *FarmRender_GetTileStateName(TileState state);
const char *FarmRender_GetTileHint(const FarmState *state, const CropTile *tile);

void FarmRender_RenderRules(uint32_t now_ms);
void FarmRender_RenderField(const FarmState *state);

#endif