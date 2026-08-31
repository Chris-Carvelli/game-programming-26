#include <SDL3/SDL.h>

const int WINDOW_W = 800;
const int WINDOW_H = 600;
const SDL_Time TARGET_FPS               = 60;
const SDL_Time TARGET_FRAMERATE_NS      = 1000000000 / TARGET_FPS; // 16666666 nanoseconds
const SDL_Time SLEEP_SAFETY_MARGIN_NS   = 1000000;                 // 1 millisecond
const SDL_Time ARTIFICIAL_DELAY_NS_MAX  = TARGET_FRAMERATE_NS *2; 
const SDL_Time ARTIFICIAL_DELAY_NS_INCR = 1000000;                 // 1 millisecond

const Sint64 ENTITIES_MAX  =  1000000;
const Sint64 ENTITIES_INCR =     1000;

struct EntityData
{
	SDL_FRect  rect;
	SDL_Color  color;
	SDL_FPoint velocity_fluid;
	SDL_FPoint velocity_fixed;
};

void entities_init(EntityData* entities);
void entities_update_fluid(SDL_Renderer* renderer, EntityData* entities, Uint64 entities_count, float delta);
void entities_update_fixed(SDL_Renderer* renderer, EntityData* entities, Uint64 entities_count);

int main(int argc, char* argv[])
{
	EntityData* entities = (EntityData*)SDL_calloc(ENTITIES_MAX, sizeof(EntityData));
	Uint64 entities_count = ENTITIES_INCR;
	bool use_fixed_framerate = false;
	SDL_Time artificial_delay = 0;

	bool quit = false;
	SDL_Window* window = SDL_CreateWindow("03_capped_loop", WINDOW_W, WINDOW_H, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// measurement needed for loop synchronization
	SDL_Time walltime_frame_beg = 0;
	SDL_Time walltime_frame_end = 0;
	SDL_Time walltime_work_end  = 0;
	SDL_Time time_elapsed_work  = 0;
	SDL_Time time_elapsed_frame = 0;

	// additional measurements for loggind and profiling
	// (some of this might be redundant but left for legibility)
	SDL_Time walltime_sleep_beg    = 0;
	SDL_Time walltime_sleep_end    = 0;
	SDL_Time walltime_busywait_beg = 0;
	SDL_Time walltime_busywait_end = 0;
	SDL_Time time_elapsed_sleep    = 0;
	SDL_Time time_elapsed_busywait = 0;

	// NOTE: timing computation is in milliseconds or nanoseconds (and integer) for precision
	//       but for the gameplay we prefer it in seconds (and as a float), since for gameplay
	//       design and physics equations are much easier
	float delta_s = 0;

	entities_init(entities);

	// NOTE: initialize 
	SDL_GetCurrentTime(&walltime_frame_end);

	while(!quit)
	{
		walltime_frame_beg = walltime_frame_end;
		
		// input
		SDL_Event event = { 0 };
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_EVENT_QUIT:
					quit = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					if(event.key.key == SDLK_F1 && !event.key.repeat)
						use_fixed_framerate = !use_fixed_framerate;

					if(event.key.key == SDLK_Q && entities_count > 0)
						entities_count -= ENTITIES_INCR;
					if(event.key.key == SDLK_E && entities_count < ENTITIES_MAX)
						entities_count += ENTITIES_INCR;
					if(event.key.key == SDLK_W)
						entities_count = ENTITIES_INCR;

					if(event.key.key == SDLK_A && artificial_delay > 0)
						artificial_delay -= ARTIFICIAL_DELAY_NS_INCR;
					if(event.key.key == SDLK_D && entities_count < ARTIFICIAL_DELAY_NS_MAX)
						artificial_delay += ARTIFICIAL_DELAY_NS_INCR;
					if(event.key.key == SDLK_S)
						artificial_delay = 0;

					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		if(use_fixed_framerate)
			entities_update_fixed(renderer, entities, entities_count);
		else
			entities_update_fluid(renderer, entities, entities_count, delta_s);

		if(artificial_delay > 0)
			SDL_DelayNS(artificial_delay);

		// render timing info from PREVIOUS frame
		{
			SDL_FRect rect_bg = SDL_FRect{ 5, 5, WINDOW_W - 5, 145 };
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x92);
			SDL_RenderFillRect(renderer, &rect_bg);

			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderDebugTextFormat(renderer, 10.0f,  10.0f, "delta          : %9.8f s", delta_s);
			SDL_RenderDebugTextFormat(renderer, 10.0f,  20.0f, "elapsed (frame): %9.6f ms", (float)time_elapsed_frame/(float)1000000);
			SDL_RenderDebugTextFormat(renderer, 10.0f,  30.0f, "elapsed (work) : %9.6f ms", (float)time_elapsed_work/(float)1000000);
			SDL_RenderDebugTextFormat(renderer, 10.0f,  40.0f, "time spent sleeping   : %9.6f ms", (float)time_elapsed_sleep/(float)1000000);
			SDL_RenderDebugTextFormat(renderer, 10.0f,  50.0f, "time spent busywaiting: %9.6f ms", (float)time_elapsed_busywait/(float)1000000);

			SDL_RenderDebugTextFormat(renderer, 10.0f,  70.0f, "[F1] Loop type: %s", use_fixed_framerate ? "FIXED" : "FLUID CAPPED");
			SDL_RenderDebugTextFormat(renderer, 10.0f,  80.0f, "alive entitites: %7lu/%lu", entities_count, ENTITIES_MAX);
			SDL_RenderDebugTextFormat(renderer, 10.0f,  90.0f, "artificial_delay: %9.6f ms", (float)artificial_delay/(float)1000000);

			SDL_RenderDebugTextFormat(renderer, 10.0f, 110.0f, "           entities   artifical_delay");
			SDL_RenderDebugTextFormat(renderer, 10.0f, 120.0f, "increase   Q          A");
			SDL_RenderDebugTextFormat(renderer, 10.0f, 130.0f, "decrease   E          S");
			SDL_RenderDebugTextFormat(renderer, 10.0f, 140.0f, "reset      W          D");
		}

		SDL_RenderPresent(renderer);

		SDL_GetCurrentTime(&walltime_work_end);

		time_elapsed_work = walltime_work_end - walltime_frame_beg;


		if(TARGET_FRAMERATE_NS > time_elapsed_work + SLEEP_SAFETY_MARGIN_NS)			
		{
			SDL_GetCurrentTime(&walltime_sleep_beg);
			SDL_DelayNS(TARGET_FRAMERATE_NS - time_elapsed_work - SLEEP_SAFETY_MARGIN_NS);
			SDL_GetCurrentTime(&walltime_sleep_end);
		}
		else
			walltime_sleep_beg = walltime_sleep_end = walltime_work_end;

		// NOTE: chained assignment! All variables in the chain will have the value of the
		//       rightmost one! Another one of those old school C patterns that you see from
		//       time to time. This is equivalent to
		// walltime_busywait_beg = walltime_sleep_end;
		// walltime_busywait_end = walltime_sleep_end;
		walltime_busywait_beg = walltime_busywait_end = walltime_sleep_end;

		while(walltime_busywait_end - walltime_frame_beg < TARGET_FRAMERATE_NS)
			SDL_GetCurrentTime(&walltime_busywait_end);

		SDL_GetCurrentTime(&walltime_frame_end);

		// compute elapsed times for frame that just ended
		// NOTE: we already calculated `time_elapsed_work` above to decide how much to sleep
		time_elapsed_frame    = walltime_frame_end    - walltime_frame_beg;
		time_elapsed_busywait = walltime_busywait_end - walltime_busywait_beg;
		time_elapsed_sleep    = walltime_sleep_end    - walltime_sleep_beg;

		delta_s = (float)time_elapsed_frame / 1000000000;
	}

	SDL_Quit();
	
	return 0;
};

void entities_init(EntityData* entities)
{
	// set a fixed random seed for reproducibility, or use 0 to get an actual random sequence
	SDL_srand(424242);

	const int size = 10;
	const float speed_fluid = 150;                      // in pixels/second
	const float speed_fixed = speed_fluid / TARGET_FPS; // in pixels/frame
	SDL_Log("entities speed    fixed: %f      fluid: %f", speed_fixed, speed_fluid);

	for(Uint64 i = 0; i < ENTITIES_MAX; ++i)
	{
		EntityData* entity = &entities[i];
		entity->rect.x = SDL_rand(WINDOW_W);
		entity->rect.y = SDL_rand(WINDOW_H);
		entity->rect.w = size;
		entity->rect.h = size;
		entity->color.r = SDL_rand(256);
		entity->color.g = SDL_rand(256);
		entity->color.b = SDL_rand(256);
		entity->color.a = 0xff;

		// NOTE: this is not physically accurate! Here we just want a bit of a random range
		//       Check out E04 for the relation between speed and velocity
		SDL_FPoint direction;
		direction.x = (SDL_randf() - 0.5f) * 2;
		direction.y = (SDL_randf() - 0.5f) * 2;
		float len = SDL_sqrt(direction.x*direction.x + direction.y*direction.y);
		direction.x /= len;
		direction.y /= len;
		
		entity->velocity_fluid.x = speed_fluid * direction.x;
		entity->velocity_fluid.y = speed_fluid * direction.y;
		entity->velocity_fixed.x = speed_fixed * direction.x;
		entity->velocity_fixed.y = speed_fixed * direction.y;
	}
}

void entities_update_fluid(SDL_Renderer* renderer, EntityData* entities, Uint64 entities_count, float delta)
{
	for(Uint64 i = 0; i < entities_count; ++i)
	{
		EntityData* entity = &entities[i];
		entity->rect.x += entity->velocity_fluid.x * delta;
		entity->rect.y += entity->velocity_fluid.y * delta;

		if(entity->rect.x < 0 || entity->rect.x > WINDOW_W)
		{
			entity->velocity_fixed.x *= -1;
			entity->velocity_fluid.x *= -1;
		}

		if(entity->rect.y < 0 || entity->rect.y > WINDOW_H)
		{
			entity->velocity_fixed.y *= -1;
			entity->velocity_fluid.y *= -1;
		}

		SDL_SetRenderDrawColor(renderer, entity->color.r, entity->color.g, entity->color.b, entity->color.a);
		SDL_RenderFillRect(renderer, &entity->rect);
	}
}

void entities_update_fixed(SDL_Renderer* renderer, EntityData* entities, Uint64 entities_count)
{
	for(Uint64 i = 0; i < entities_count; ++i)
	{
		EntityData* entity = &entities[i];
		entity->rect.x += entity->velocity_fixed.x;
		entity->rect.y += entity->velocity_fixed.y;

		if(entity->rect.x < 0 || entity->rect.x > WINDOW_W)
		{
			entity->velocity_fixed.x *= -1;
			entity->velocity_fluid.x *= -1;
		}

		if(entity->rect.y < 0 || entity->rect.y > WINDOW_H)
		{
			entity->velocity_fixed.y *= -1;
			entity->velocity_fluid.y *= -1;
		}

		SDL_SetRenderDrawColor(renderer, entity->color.r, entity->color.g, entity->color.b, entity->color.a);
		SDL_RenderFillRect(renderer, &entity->rect);
	}
}