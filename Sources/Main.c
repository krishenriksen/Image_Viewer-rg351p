/** @file Main.c
 * Program initialization and main loop.
 * @author Adrien RICCIARDI
 * @modified Kris Henriksen
 */
#include <Configuration.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Viewport.h>

// Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

// Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

/** Automatically called on program exit, gracefully uninitialize SDL. */
static void MainExit(void) {
	// Close game controller
	SDL_JoystickClose( gGameController );
	gGameController = NULL;

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

static char String_Program_Title[256];
static SDL_Surface *Pointer_Surface_Image;

void ViewportChangeImage(char *img) {
	// Create window title from image name
	strcpy(String_Program_Title, "Image Viewer - ");
	strncat(String_Program_Title, img, sizeof(String_Program_Title) - sizeof("Image Viewer - "));	

	SDL_Surface *Pointer_Surface_Image = IMG_Load(img);
	ViewportInitialize(String_Program_Title, Pointer_Surface_Image);

	SDL_FreeSurface(Pointer_Surface_Image);
}

int main(int argc, char *argv[]) {
	SDL_Event Event;
	unsigned int Frame_Starting_Time = 0, Elapsed_Time;
	int Mouse_X, Mouse_Y, Zoom_Factor = 1, i;
	TViewportFlippingModeID Flipping_Mode = VIEWPORT_FLIPPING_MODE_ID_NORMAL;

	// current viewed image index
	int current_image_index = 1;

	// Check arguments
	if (argc < 2) {
		return EXIT_FAILURE;
	}

	// Initialize SDL before everything else, so other SDL libraries can be safely initialized
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
		printf("Error : failed to initialize SDL (%s).\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Check for joysticks
	if (SDL_NumJoysticks() < 1) {
		printf( "Warning: No joysticks connected!\n" );
		return EXIT_FAILURE;
	}
	else {
		// Load joystick
		gGameController = SDL_JoystickOpen( 0 );
		if (gGameController == NULL) {
			printf( "Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
			return EXIT_FAILURE;
		}
	}

	atexit(MainExit);

	// Try to initialize the SDL image library
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) != (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) {
		printf("Error : failed to initialize SDL image library (%s).\n", IMG_GetError());
		return EXIT_FAILURE;
	}


	// Create window title from image name
	strcpy(String_Program_Title, "Image Viewer - ");
	strncat(String_Program_Title, argv[1], sizeof(String_Program_Title) - sizeof("Image Viewer - "));	

	Pointer_Surface_Image = IMG_Load(argv[1]);
	if (ViewportInitialize(String_Program_Title, Pointer_Surface_Image) != 0) return EXIT_FAILURE;

	SDL_FreeSurface(Pointer_Surface_Image);

	// Process incoming SDL events
	while (1) {
		// Keep the time corresponding to the frame rendering beginning
		Frame_Starting_Time = SDL_GetTicks();

		while (SDL_PollEvent(&Event)) {
			switch (Event.type) {
				case SDL_QUIT:
					return EXIT_SUCCESS;

				case SDL_WINDOWEVENT:
					// Tell the viewport that its size changed
					if (Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						ViewportSetDimensions(Event.window.data1, Event.window.data2);
						Zoom_Factor = 1; // Zoom has been reset when resizing the window
					}

					break;

				case SDL_MOUSEWHEEL:
					// Wheel is rotated toward the user, increment the zoom factor
					if (Event.wheel.y > 0) {
						if (Zoom_Factor < CONFIGURATION_VIEWPORT_MAXIMUM_ZOOM_FACTOR) {
							Zoom_Factor *= 2;
						}
					}
					else {
						// Wheel is rotated away from the user, decrement the zoom factor
						if (Zoom_Factor > 1) {
							Zoom_Factor /= 2;
						}
					}
		
					// Start zooming area from the mouse coordinates
					SDL_GetMouseState(&Mouse_X, &Mouse_Y);
					ViewportSetZoomedArea(Mouse_X, Mouse_Y, Zoom_Factor);

					break;

				case SDL_KEYDOWN:
					// Quit program
					if (Event.key.keysym.sym == SDLK_q) {
						return EXIT_SUCCESS;
					}
					
					// Toggle image flipping
					if (Event.key.keysym.sym == SDLK_f) {
						// Set next available flipping mode
						Flipping_Mode++;
						if (Flipping_Mode >= VIEWPORT_FLIPPING_MODE_IDS_COUNT) {
							Flipping_Mode = 0;
						}

						ViewportSetFlippingMode(Flipping_Mode);

						// Zoom has been reset when flipping the image
						Zoom_Factor = 1;
					}
					
					break;

					case SDL_MOUSEMOTION:
						// Do not recompute everything when the image is not zoomed
						if (Zoom_Factor > 1) {
							// Successively zoom to the current zoom level to make sure the internal ViewportSetZoomedArea() data are consistent
							for (i = 1; i <= Zoom_Factor; i <<= 1) ViewportSetZoomedArea(Event.motion.x, Event.motion.y, i);
						}
						
						break;

					// buttons
					case SDL_JOYBUTTONDOWN:
			            // select
			            if (Event.jbutton.button == 7) {
			            	return EXIT_SUCCESS;
			            }

						// start
						if (Event.jbutton.button == 6) {   
							return EXIT_SUCCESS;
						}

						// a - Toggle image flipping
						if (Event.jbutton.button == 0) {
							// Set next available flipping mode
							Flipping_Mode++;
							if (Flipping_Mode >= VIEWPORT_FLIPPING_MODE_IDS_COUNT) {
								Flipping_Mode = 0;
							}

							ViewportSetFlippingMode(Flipping_Mode);

							// Zoom has been reset when flipping the image
							Zoom_Factor = 1;		
						}

						break;

					// d-pad
					case SDL_JOYHATMOTION:
						// up
						if (Event.jhat.value == 1) {
							if (current_image_index > 1) {
								current_image_index -= 1;
								ViewportChangeImage(argv[current_image_index]);
							}
						}

						// down
						if (Event.jhat.value == 4) {
							if (current_image_index < (argc - 1)) {
								current_image_index += 1;
								ViewportChangeImage(argv[current_image_index]);
							}
						}

						break;

					// Unhandled event, do nothing
					default:
						break;
			}
		}

		ViewportDrawImage();

		// Wait enough time to get a 60Hz refresh rate
		Elapsed_Time = SDL_GetTicks() - Frame_Starting_Time;
		if (Elapsed_Time < CONFIGURATION_DISPLAY_REFRESH_RATE_PERIOD) {
			SDL_Delay(CONFIGURATION_DISPLAY_REFRESH_RATE_PERIOD - Elapsed_Time);
		}
	}
}
