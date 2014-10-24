#include <stdio.h>
#include <unistd.h>
#include "camera.h"
#include "graphics.h"
#include <opencv/cv.hpp>


float minTargetRadius = ((float)MAIN_TEXTURE_WIDTH)*0.05;
bool do_thresholding = true;
int cap_width =  320;
int cap_height = 320;

using namespace std;

int main(int argc, const char **argv)
{

	if (argc > 1 )
	{
		do_thresholding = false;
	}
	//init graphics and the camera
	InitGraphics();
	CCamera* cam = StartCamera(cap_width, cap_height, 30, 1, true);

    GfxTexture texture;
	texture.Create(cap_width, cap_height);


	cv::Mat frame;

	printf("Running frame loop\n");
	while (true)
	{

		//lock the chosen frame buffer, and copy it directly into the corresponding open gl texture
		const void* frame_data; int frame_sz;
		if(cam->BeginReadFrame(0,frame_data,frame_sz))
		{
			//if doing argb conversion the frame data will be exactly the right size so just set directly
			frame = cv::Mat(cap_width, MAIN_TEXTURE_WIDTHcap_height, CV_8UC4, (void*)frame_data);
			if (do_thresholding)
			{
				cv::cvtColor(frame, frame, CV_RGB2HSV); 
			}

			cam->EndReadFrame(0);

			if (do_thresholding)
			{
				cv::inRange(frame, cv::Scalar(300 / 2, 100, 100), cv::Scalar(360 / 2, 255, 255), frame); 
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
					
					for( int i=0; i<count; i++)
					{
						cv::Point2f c;
						float r;
						cv::minEnclosingCircle( contours[i], c, r);
						
						if ( r >= minTargetRadius)
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
					printf("circle at %d, %d - radius %d\n", center[i].x, center[i].y, radius[i]);
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
	}

	StopCamera();
}
