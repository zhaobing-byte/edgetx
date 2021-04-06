/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include "sliders.h"
#include "trims.h"

constexpr coord_t border = 10;

const uint8_t LBM_LAYOUT_2x1[] = {
#include "mask_layout2x1.lbm"
};

const ZoneOption OPTIONS_LAYOUT_2x1[] =  {
  LAYOUT_COMMON_OPTIONS,
  LAYOUT_OPTIONS_END
};

class Layout2x1: public Layout
{
  public:
    Layout2x1(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    void create() override
    {
      Layout::create();
      persistentData->options[0].value.boolValue = true;
      persistentData->options[1].value.boolValue = true;
      persistentData->options[2].value.boolValue = true;
      persistentData->options[3].value.boolValue = true;
      persistentData->options[4].value.boolValue = false;
      persistentData->options[5].value.boolValue = false;
      decorate();
    }

    unsigned int getZonesCount() const override
    {
      return 2;
    }

    rect_t getZone(unsigned int index) const override
    {
      if (isMirrored())
        index = 1 - index;

      rect_t zone = getMainZone({border, border, LCD_W - 2 * border, LCD_H - 2 * border});

      if (index == 0) {
        return {zone.x, zone.y, zone.w / 2, zone.h};
      }
      else {
        return {zone.x + zone.w / 2, zone.y, zone.w / 2, zone.h};
      }

      return zone;
    }
};

BaseLayoutFactory<Layout2x1> Layout2x1("Layout2x1", "2 x 1", LBM_LAYOUT_2x1, OPTIONS_LAYOUT_2x1);
