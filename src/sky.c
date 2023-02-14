#include "sky.h"
#include "stdio.h"

const vec3 DAY_COLOR = {0.6f, 0.7f, 1.0f};
const vec3 NIGHT_COLOR = {0.0f, 0.0f, 0.0f};

void sky_init(struct Sky *sky)
{
    sky->curr_color = DAY_COLOR;
    sky->ticks = 0;
};

// 0 -> 2400 = day
// 2400 -> 0 = night
void sky_tick(struct Sky *sky)
{
    sky->ticks++;
    // if its night (ticks > 2400) then we want to gradually switch into day, then reset ticks
    if (sky->ticks > 2400)
    {
        sky->curr_color.x = lerp(NIGHT_COLOR.x, DAY_COLOR.x, (float)(sky->ticks - 2400) / 2400.0f);
        sky->curr_color.y = lerp(NIGHT_COLOR.y, DAY_COLOR.y, (float)(sky->ticks - 2400) / 2400.0f);
        sky->curr_color.z = lerp(NIGHT_COLOR.z, DAY_COLOR.z, (float)(sky->ticks - 2400) / 2400.0f);
        if (sky->ticks > 4800)
        {
            sky->ticks = 0;

            // reset the color to day
            sky->curr_color = DAY_COLOR;
        }
    }
    else
    {
        sky->curr_color.x = lerp(DAY_COLOR.x, NIGHT_COLOR.x, (float)sky->ticks / 2400.0f);
        sky->curr_color.y = lerp(DAY_COLOR.y, NIGHT_COLOR.y, (float)sky->ticks / 2400.0f);
        sky->curr_color.z = lerp(DAY_COLOR.z, NIGHT_COLOR.z, (float)sky->ticks / 2400.0f);
    }
};

void sky_render(struct Sky *sky)
{
    glClearColor(sky->curr_color.x, sky->curr_color.y, sky->curr_color.z, 1.0f);
}