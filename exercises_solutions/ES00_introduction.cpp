#include <SDL3/SDL.h>

int main(void)
{
	
	// toggle to swith between the insulated player update (aka, the way you want to do it)
	// and the one performed immediate after polling the event queue
	bool use_insulated_player_update = true;

	float window_w = 800;
	float window_h = 600;
	int target_framerate_ms = 1000 / 60;       // 16 milliseconds
	int target_framerate_ns = 1000000000 / 60; // 16666666 nanoseconds

	SDL_Window* window = SDL_CreateWindow("ES00 - introduction (solved)", window_w, window_h, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, "vulkan");

	// increase the zoom to make debug text more legible
	// (ie, on the class projector, we will usually use 2)
	{
		float zoom = 1;
		window_w /= zoom;
		window_h /= zoom;
		SDL_SetRenderScale(renderer, zoom, zoom);
	}

	bool quit = false;

	SDL_Time walltime_frame_beg = 0;
	SDL_Time walltime_work_end  = 0;
	SDL_Time walltime_frame_end = 0;
	SDL_Time time_elapsed_frame = 0;
	SDL_Time time_elapsed_work  = 0;

	SDL_Time time_elapsed_sleep;
	SDL_Time time_elapsed_busywait;

	int delay_type = 0;

	float player_speed = 2;
	float player_size = 40;
	SDL_FRect player_rect;
	player_rect.w = player_size;
	player_rect.h = player_size;
	player_rect.x = window_w / 2 - player_size / 2;
	player_rect.y = window_h / 2 - player_size / 2;

	// NOTE: list of stuff with the same prefix? Looks like it's a good candidate for consolidation
	bool btn_pressed_up    = false;
	bool btn_pressed_down  = false;
	bool btn_pressed_left  = false;
	bool btn_pressed_right = false;


	SDL_GetCurrentTime(&walltime_frame_beg);
	while(!quit)
	{
		// input
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_EVENT_QUIT:
					quit = true;
					break;

				// NOTE: when there is no break, both switch cases will execute the same code
				//       block. These kind of "clever" solutions can become messy very fast.
				//       We will soon move it to a more appropriate function
				//       (with a more solid event parsing).
				case SDL_EVENT_KEY_UP:
				case SDL_EVENT_KEY_DOWN:
				{
					// player inputs
					// NOTE: OS and hardware will notify events at their own pace, re-triggering
					//       events and other shenanigans. We want to insulate our game code
					//       from this, so here we will just keep track of events happening and
					//       do ACTUAL updates later in the loop
					if(use_insulated_player_update)
					{
						// insulated movement: store the fact that an event happened, so we can
						// use it at the appropriate time during update
						if(event.key.key == SDLK_W) btn_pressed_up    = event.key.down;
						if(event.key.key == SDLK_S) btn_pressed_down  = event.key.down;
						if(event.key.key == SDLK_A) btn_pressed_left  = event.key.down;
						if(event.key.key == SDLK_D) btn_pressed_right = event.key.down;
					}
					else
					{
						// raw movement: compute player position immediately.
						// THIS IS NOT HOW YOU WANT TO DO IT! Everything regarding the game
						// should be decoupled from external events
						if(event.key.key == SDLK_W) player_rect.y -= player_speed;
						if(event.key.key == SDLK_S) player_rect.y += player_speed;
						if(event.key.key == SDLK_A) player_rect.x -= player_speed;
						if(event.key.key == SDLK_D) player_rect.x += player_speed;
					}

					// debug utilities
					if (event.key.down)
					{
						// common trick to convert a key range to an array inde index
						// (works because if we look up the values of `SDLK_1` & co. we see that they are all contiguous!)
						if(event.key.key >= SDLK_1 && event.key.key < SDLK_6)
							delay_type = event.key.key - SDLK_1;
						if(event.key.key == SDLK_F1)
							use_insulated_player_update = !use_insulated_player_update;
					}
					break;
				}
			}
		}

		// clear screen
		// NOTE: `0x` prefix means we are expressing the number in hexadecimal (base 16)
		//       `0b` is another useful prefix, expresses the number in binary
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		// SDL_Delay(500);


		if(use_insulated_player_update)
		{
			// update player position
			if(btn_pressed_up)    player_rect.y -= player_speed;
			if(btn_pressed_down)  player_rect.y += player_speed;
			if(btn_pressed_left)  player_rect.x -= player_speed;
			if(btn_pressed_right) player_rect.x += player_speed;
		}

		SDL_SetRenderDrawColor(renderer, 0x3C, 0x63, 0xFF, 0XFF);
		SDL_RenderFillRect(renderer, &player_rect);

			SDL_GetCurrentTime(&walltime_work_end);
		time_elapsed_work = walltime_work_end - walltime_frame_beg;
		walltime_frame_end = walltime_work_end;

		if(target_framerate_ns > time_elapsed_work)
		{
			switch(delay_type)
			{
				case 0:
				{
					// busy wait - very precise, but costly
					while(walltime_frame_end - walltime_frame_beg < target_framerate_ns)
						SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = walltime_frame_end - walltime_work_end;
					time_elapsed_sleep = 0;
					break;
				}
				case 1:
				{
					// simple delay - too imprecise
					// NOTE: `SDL_Delay` gets milliseconds, but our timer gives us nanoseconds! We need to covert it manually
					SDL_Delay((target_framerate_ns - time_elapsed_work) / 1000000);
					SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = 0;
					time_elapsed_sleep = walltime_frame_end - walltime_frame_beg;
					break;
				}
				case 2:
				{
					// delay ns - also too imprecise
					SDL_DelayNS(target_framerate_ns - time_elapsed_work);
					SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = 0;
					time_elapsed_sleep = walltime_frame_end - walltime_frame_beg;
					break;
				}
				case 3:
				{
					// delay precise
					SDL_DelayPrecise(target_framerate_ns - time_elapsed_work);
					SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = 0;
					time_elapsed_sleep = walltime_frame_end - walltime_frame_beg;
					break;
				}
				case 4:
				{
					// custom delay - we use the sleeping delay with an arbitrary margin, then we busywait what's left
					const Uint64 sleep_safety_margin = 1000000; // ie, 1ms
					SDL_Time walltime_sleep_end;
					
					SDL_DelayNS(target_framerate_ns - time_elapsed_work - sleep_safety_margin);
					SDL_GetCurrentTime(&walltime_sleep_end);
					walltime_frame_end = walltime_sleep_end;

					while(walltime_frame_end - walltime_frame_beg < target_framerate_ns)
						SDL_GetCurrentTime(&walltime_frame_end);

					time_elapsed_busywait = walltime_frame_end - walltime_sleep_end;
					time_elapsed_sleep = walltime_sleep_end - walltime_work_end;
					break;
				}
			}
		}

		time_elapsed_frame = walltime_frame_end - walltime_frame_beg;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 10.0f, "elapsed (frame): %9.6f ms", (float)time_elapsed_frame/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 20.0f, "elapsed (work) : %9.6f ms", (float)time_elapsed_work/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 30.0f, "delay type: %d (change with 1-5)", delay_type + 1);

		SDL_RenderDebugTextFormat(renderer, 10.0f, 50.0f, "time spent sleeping   : %9.6f ms", (float)time_elapsed_sleep/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 60.0f, "time spent busywaiting: %9.6f ms", (float)time_elapsed_busywait/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 70.0f, "update type (toggle with F1) : %9s", use_insulated_player_update ? "INSULATED" : "IMMEDIATE");

		// render
		SDL_RenderPresent(renderer);
		
		// NOTE: while taking the time two different times is no ideal, in the current setup we have a problem:
		//       our `time_elapsed_frame` doesn't take into account the time it takes to render the debug view, AND
		//       to "present" the graphics. Usually that is not a big deal, but if if the untimed stuff takes too long
		//       we end up in a death spiral! Our options are:
		//       - take the time again after everything has been done (which means the time logged will be slightly lower)
		//       - show the elapsed time from the previous frame
		//       we will go with the first option here
		// walltime_frame_beg = walltime_frame_end;
		SDL_GetCurrentTime(&walltime_frame_beg);
	}

	// NOTE: we created a bunch of resources (window, renderer). Should we explicitely destroy them here?
	//       it's actually not a trivial question!

	return 0;
};


