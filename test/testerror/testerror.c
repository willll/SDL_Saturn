
/* Simple test of the SDL threading code and error handling */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "SDL.h"
#include "SDL_thread.h"

static int alive = 0;

int ThreadFunc(void *data)
{
	/* Set the child thread error string */
	SDL_SetError("Thread %s (%d) had a problem: %s",
								(char *)data, SDL_ThreadID(), "nevermind");
	while ( alive ) {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Thread '%s' is alive!\n", (char *)data);
		SDL_Delay(1*1000);
	}
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Child thread error string: %s\n", SDL_GetError());
	return(0);
}

int main(int argc, char *argv[])
{
	SDL_Thread *thread;

	/* Load the SDL library */
	if ( SDL_Init(0) < 0 ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);

	/* Set the error value for the main thread */
	SDL_SetError("No worries");

	alive = 1;
	thread = SDL_CreateThread(ThreadFunc, "#1");
	if ( thread == NULL ) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,  "Couldn't create thread: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_Delay(5*1000);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Waiting for thread #1\n");
	alive = 0;
	SDL_WaitThread(thread, NULL);

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Main thread error string: %s\n", SDL_GetError());

	return(0);
}
