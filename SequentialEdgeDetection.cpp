#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb/stb_image_write.h"

#include "SequentialEdgeDetection.h"
#include <filesystem>
#include <math.h>
using namespace std;

int* getPixel(unsigned char*data, int width, int height, int x, int y) {
	int rgb[3];
	rgb[0] = data[3 * (y * width + x)];
	rgb[1] = data[3 * (y * width + x) + 1];
	rgb[2] = data[3 * (y * width + x) + 2];
	return rgb;
}

int* addPixel(int* add1, int* add2)
{
	int px[3];
	px[0] = add1[0] + add2[0];
	px[1] += add1[1] + add2[1];
	px[2] += add1[2] + add2[2];
	return px;
}

int main(int argc, char** argv)
{	
	std::string path;
	if (argc > 0)
	{
		path = argv[1];
	}

	int width, height, original_no_channels;
	int desired_no_channels = 3;
	filesystem::create_directories("output");

	float LuminanceConv[3] = { 0.2125f, 0.7154f, 0.0721f };
	const auto start = std::chrono::steady_clock::now();
	for (auto const& dir_entry : filesystem::directory_iterator{ path })
	{
		string file = dir_entry.path().string();
		unsigned char* img = stbi_load(file.c_str(), &width, &height, &original_no_channels, desired_no_channels);
		if (img == NULL) {
			printf("Error in loading the image\n");
			exit(1);
		}
		else {
			uint8_t* ptr = (uint8_t*)malloc(width * height * (int)3);
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					int* xrgb = getPixel(img, width, height, x - 1, y - 1);

					float xr = img[3 * ((y - 1) * width + x - 1)];
					float xg = img[3 * ((y - 1) * width + x - 1) + 1];
					float xb = img[3 * ((y - 1) * width + x - 1) + 2];

					xr += 2 * img[3 * (y * width + x - 1)];
					xg += 2 * img[3 * (y * width + x - 1) + 1];
					xb += 2 * img[3 * (y * width + x - 1) + 2];

					xr +=  img[3 * ((y + 1) * width + x - 1)];
					xg +=  img[3 * ((y + 1) * width + x - 1) + 1];
					xb +=  img[3 * ((y + 1) * width + x - 1) + 2];
										 
					xr -=  img[3 * ((y - 1) * width + x + 1)];
					xg -=  img[3 * ((y - 1) * width + x + 1) + 1];
					xb -=  img[3 * ((y - 1) * width + x + 1) + 2];

					xr -= 2 * img[3 * ((y) * width + x + 1)];
					xg -= 2 * img[3 * ((y) * width + x + 1) + 1];
					xb -= 2 * img[3 * ((y) * width + x + 1) + 2];

					xr -= img[3 * ((y + 1) * width + x + 1)];
					xg -= img[3 * ((y + 1) * width + x + 1) + 1];
					xb -= img[3 * ((y + 1) * width + x + 1) + 2];

					float yr = img[3 * ((y - 1) * width + x - 1)];
					float yg = img[3 * ((y - 1) * width + x - 1) + 1];
					float yb = img[3 * ((y - 1) * width + x - 1) + 2];

					yr += 2 * img[3 * ((y - 1)* width + x)];
					yg += 2 * img[3 * ((y - 1)* width + x) + 1];
					yb += 2 * img[3 * ((y - 1)* width + x) + 2];

					yr += img[3 * ((y - 1) * width + x + 1)];
					yg += img[3 * ((y - 1) * width + x + 1) + 1];
					yb += img[3 * ((y - 1) * width + x + 1) + 2];

					yr -= img[3 * ((y + 1) * width + x - 1)];
					yg -= img[3 * ((y + 1) * width + x - 1) + 1];
					yb -= img[3 * ((y + 1) * width + x - 1) + 2];
					
					yr -= 2 * img[3 * ((y + 1)*width + x)];
					yg -= 2 * img[3 * ((y + 1)*width + x) + 1];
					yb -= 2 * img[3 * ((y + 1)*width + x) + 2];
					
					yr -= img[3 * ((y + 1) * width + x + 1)];
					yg -= img[3 * ((y + 1) * width + x + 1) + 1];
					yb -= img[3 * ((y + 1) * width + x + 1) + 2];

					xr *= LuminanceConv[0];
					xg *= LuminanceConv[1];
					xb *= LuminanceConv[2];
					yr *= LuminanceConv[0];
					yg *= LuminanceConv[1];
					yb *= LuminanceConv[2];

					ptr[3 * (y * width + x)] = max(min(sqrt(xr * xr + yr * yr), 255.0f), 0.0f);
					ptr[3 * (y * width + x) + 1] = max(min(sqrt(xg * xg + yg * yg), 255.0f), 0.0f);
					ptr[3 * (y * width + x) + 2] = max(min(sqrt(xb * xb + yb * yb), 255.0f), 0.0f);
				}
			}
			char new_path[_MAX_PATH];
			
			auto path_fs = filesystem::current_path() / "output" / dir_entry.path().filename();
			stbi_write_jpg(path_fs.string().c_str(), width, height, 3, ptr, 100);
		}
		free(img);
	}
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "seconds: " << elapsed_seconds.count() << '\n';
	return 0;
}
