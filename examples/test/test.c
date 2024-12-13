/**
 * (unlicensed, not part of the Dodge Gaming Library)
 * This program demonstrates rotation, inversion, and entities.
 * 
 * cc test.c -lDGL -lGL -lm -lpthread -ldl -lrt -lX11
 */

#include <dgl.h>
#include <stdlib.h>
#include <stdio.h>

sprite a, b, c, d, one, two, three, background;
entity ent;
int main()
{
	start("./DGLsession.dat");
	init_sprite(&a, 20, 20, 220, 220, pixmap_generate(RECT, shinf(20, 20), RED, NULL));
	init_sprite(&b, 20, 20, 220, 220, pixmap_generate(RECT, shinf(20, 20), PURPLE, NULL));
	init_sprite(&c, 20, 20, 220, 220, pixmap_generate(RECT, shinf(20, 20), GREEN, NULL));
	init_sprite(&d, 20, 20, 220, 220, pixmap_generate(RECT, shinf(20, 20), BLUE, NULL));
	init_sprite(&one, 30, 30, 0, 30 /*offset from entity*/, pixmap_generate(RECT, shinf(30, 30), BROWN, NULL));
	init_sprite(&two, 30, 30, 0, 0 /*offset from entity*/, pixmap_generate(EQUILATERAL, shinf(30, 30), GREEN, NULL));
	init_sprite(&three, 30, 30, 200, 400, two.pixmap);
	init_sprite(&background, windowWidth, windowHeight, 0, 0, pixmap_generate(RECT, shinf(windowWidth, windowHeight), WHITE, NULL));

	insert_sprite(&a, 1, MAX_SPRITES - 1);
	add_sprite(&b, 1);
	add_sprite(&c, 1);
	add_sprite(&d, 1);
	add_sprite(&one, 2);
	add_sprite(&two, 2);
	add_sprite(&three, 2);
	add_sprite(&background, 0);

	a.negate_x = true;
	a.negate_y = true;

	b.negate_x = false;
	b.negate_y = true;

	c.negate_x = true;
	c.negate_y = false;
	screen_dat.pause_render = false;
	int i = 0;

	add_sprites_to_entity(&ent, &one, &two, NULL);
	ent.y = 410;
	while(running)
	{
		a.rotation = i % 360;
		b.rotation = -i % 360;
		c.rotation = -i % 360;
		d.rotation = i % 360;
		i++;
		
		ent_update_collision(&ent, 2);

		if(!ent.entity_collision)
			ent.x++;

		compile();
	}

	delete_sprite(&a);
	delete_sprite(&background);
	exit(0);
}