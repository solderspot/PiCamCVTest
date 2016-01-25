#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include "camera.h"
#include "graphics.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define DEFAULT_HUE_MIN 0
#define DEFAULT_HUE_MAX 60
#define DEFAULT_SAT_MIN 100
#define DEFAULT_SAT_MAX 255
#define DEFAULT_VAL_MIN 100
#define DEFAULT_VAL_MAX 255
#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 320



float minTargetRadiusFactor = 0.05;
bool do_thresholding = true;
int flipv = 0;
int fliph = 0;
int awbmode = 1;
int hue_min = 	DEFAULT_HUE_MIN;
int hue_max = 	DEFAULT_HUE_MAX;
int sat_min = 	DEFAULT_SAT_MIN;
int sat_max = 	DEFAULT_SAT_MAX;
int val_min = 	DEFAULT_VAL_MIN;
int val_max = 	DEFAULT_VAL_MAX;
int cap_width = DEFAULT_WIDTH;
int cap_height= DEFAULT_HEIGHT;

static bool parse_min_max( int argc, const char **argv, const char *arg, int *i, int *min_p, int *max_p, int max_val );
static bool parse_int( int argc, const char **argv, const char *arg, int *i, int *val_p );
static bool parse_args( int argc, const char **argv );
static void usage( const char *name );
static int64_t gettime( void );

using namespace std;

int main(int argc, const char **argv)
{

	if (!parse_args( argc, argv))
	{
		usage( argv[0]);
		return -1;
	}

	//init graphics and the camera
	InitGraphics();
	CCamera* cam = StartCamera(cap_width, cap_height, 30, 1, true, awbmode, flipv, fliph );

    GfxTexture texture;
	texture.Create(cap_width, cap_height);


	cv::Mat frame;

	printf("Starting capture: %dx%d\n", cap_width, cap_height );
	if ( do_thresholding )
	{
		printf("Applying color threshold: hue %d..%d, sat %d..%d, val %d..%d\n", hue_min, hue_max, sat_min, sat_max, val_min, val_max); 
	}
	if ( fliph )
	{
		printf("image flipped horizontally\n");
	}
	if ( flipv )
	{
		printf("image flipped vertically\n");
	}

    int64_t start = gettime();
	int64_t update_interval = 5*1000;
	int frames = 0;
	while (true) 
	{

		const void* frame_data; int frame_sz;
		if(cam->BeginReadFrame(0,frame_data,frame_sz))
		{
			frame = cv::Mat(cap_height, cap_width, CV_8UC4, (void*)frame_data);

			if (do_thresholding)
			{
				cv::cvtColor(frame, frame, CV_RGB2HSV); 
			}

			cam->EndReadFrame(0);

			if (do_thresholding)
			{
				cv::inRange(frame, cv::Scalar(hue_min / 2, sat_min, val_min), cv::Scalar(hue_max / 2, sat_max , val_max), frame); 
				cv::Mat str_el = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
				morphologyEx(frame, frame, cv::MORPH_OPEN, str_el);
				morphologyEx(frame, frame, cv::MORPH_CLOSE, str_el);

				std::vector<cv::Point2i> center;
				std::vector<int> radius;


				{
					std::vector<std::vector<cv::Point> > contours;
					std::vector<cv::Vec4i> heirarchy;

					cv::findContours( frame.clone(), contours, heirarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
			
					size_t count = contours.size();
					float minR = ((float)cap_width)*minTargetRadiusFactor;
					
					for( int i=0; i<count; i++)
					{
						cv::Point2f c;
						float r;
						cv::minEnclosingCircle( contours[i], c, r);
						
						if ( r >= minR)
						{
							center.push_back(c);
							radius.push_back(r);
						}
					}
				}


				cvtColor( frame, frame, CV_GRAY2RGBA);
				// draw all the bounding circles
				
				size_t count = center.size();
				
				for( int i = 0; i < count; i++)
				{
					cv::circle(frame, center[i], radius[i], cv::Scalar(255,0,0, 255), 3);
				}

			}
		}


		//begin frame, draw the texture then end frame (the bit of maths just fits the image to the screen while maintaining aspect ratio)
		BeginFrame();
		texture.SetPixels(frame.data);
		float aspect_ratio = float(cap_width)/float(cap_height);
		float screen_aspect_ratio = 1280.f/720.f;
		DrawTextureRect(&texture,-aspect_ratio/screen_aspect_ratio,-1.f,aspect_ratio/screen_aspect_ratio,1.f);
		EndFrame();

		// update frame rate results periodically
		frames++;
		int64_t now = gettime();
		int64_t interval = now - start;

		if ( interval > update_interval )
		{
			int hfps = (frames*100000)/interval;
			printf("Frame rate: %d.%02d fps\n", hfps/100, hfps%100);
			// reset
			start = now;
			frames = 0;
		}
	}

	StopCamera();
}


bool parse_args( int argc, const char **argv)
{
	for (int i = 1; i<argc; i++)
	{
		const char *arg = argv[i];

		if ( *arg++ != '-')
		{
			return false;
		}

		char sw = *arg++;

		switch ( sw )
		{
			case 'f' :
			{
				switch (*arg++)
				{
					case 'v': flipv = 1; break;
					case 'h': fliph = 1; break;
					default: return false;
				}
				break; 
			}
			case 'r':
			{
				do_thresholding = false;
				break;
			}
			case 'H':
			{
				if ( !parse_min_max( argc, argv, arg, &i, &hue_min, &hue_max, 360))
				{
					return false;
				}
				break;
			}
			case 'S':
			{
				if ( !parse_min_max( argc, argv, arg, &i, &sat_min, &sat_max, 255))
				{
					return false;
				}
				break;
			}
			case 'V':
			{
				if ( !parse_min_max( argc, argv, arg, &i, &val_min, &val_max, 255))
				{
					return false;
				}
				break;
			}
			case 'w':
			{
				if ( !parse_int( argc, argv, arg, &i, &cap_width))
				{
					return false;
				}
				break;
			}
			case 'h':
			{
				if ( !parse_int( argc, argv, arg, &i, &cap_height))
				{
					return false;
				}
				break;
			}



			default:	return false;
		}
	}
	return true;
}

bool parse_min_max( int argc, const char **argv, const char *arg, int *i, int *min_p, int *max_p, int max_val)
{
	if (*arg == 0)
	{
		if (++(*i) >= argc )
		{
			return false;
		}
		arg = argv[*i];
	}

	int min, max;
	if ( sscanf(arg, "%u..%u", &min, &max) != 2) 
	{
		return false;
	}

	*min_p = min >= 0 ?( min < max_val ? (min <= max ? min : max) : max_val) : 0;
	*max_p = max >= 0 ?( max < max_val ? (max >= min ? max : min) : max_val) : 0;

	return true;
}

bool parse_int( int argc, const char **argv, const char *arg, int *i, int *val_p )
{
	if (*arg == 0)
	{
		if (++(*i) >= argc )
		{
			return false;
		}
		arg = argv[*i];
	}
	*val_p = atoi(arg);
	return true;
}

void usage( const char *name )
{
	printf("Usage: %s [options]\n", name );
	printf("  Where options are:\n");
	printf("    -fh              :  flip image horizontally\n");
	printf("    -fv              :  flip image vertically\n");
	printf("    -w <pixels>      :  capture image width - defailt %d\n", DEFAULT_WIDTH);
	printf("    -h <pixels>      :  capture image height - defailt %d\n", DEFAULT_HEIGHT);
	printf("    -r               :  don't process input\n");
	printf("    -H <min>-<max>   :  hue range (0..360) - defailt %d..%d\n", DEFAULT_HUE_MIN, DEFAULT_HUE_MAX);
	printf("    -S <min>-<max>   :  saturation range (0..255) - defailt %d..%d\n", DEFAULT_SAT_MIN, DEFAULT_SAT_MAX);
	printf("    -V <min>-<max>   :  value range (0..255) - defailt %d..%d\n", DEFAULT_VAL_MIN, DEFAULT_VAL_MAX);
	printf("\n  example: %s -h 10..50 -s 50..255 -v 100..200\n", name);
}

int64_t gettime( void )
{
	struct timespec now;

	clock_gettime(CLOCK_REALTIME, &now);

	return (int64_t)now.tv_sec*1000 + (int64_t)now.tv_nsec/1000000;

}


