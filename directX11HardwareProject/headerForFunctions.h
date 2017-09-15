#pragma once
#include "includes.h"
#include <random>
//LERPS  BETWEEN "COLORS" BUT CAN BE USED FOR ANYTHING
float lerp(unsigned int color1, unsigned int color2, float terp)
{
	return color1 * (1 - terp) + color2 * terp;
}


void clearScreen(unsigned int arr[], float width, float height)
{
	for (unsigned int i = 0; i < height * width; i++)
	{
		arr[i] = 0x00000000;

		// array = white
	}
}

//FOR BGRA ONLY
unsigned int ColorCorrection(unsigned int colorToChange)
{//bgra
	unsigned int copyColor = colorToChange;
	unsigned int blueBits = copyColor & 0xFF000000;
	unsigned int redBits = copyColor & 0x0000FF00;
	unsigned int greenBits = copyColor & 0x00FF0000;
	unsigned int alphaBits = copyColor & 0x00000000;

	//argb
	redBits = redBits << 8;
	alphaBits = alphaBits << 24;
	greenBits = greenBits >> 8;
	blueBits = blueBits >> 24;
	//copyColor &= blueBits;
	//copyColor &= greenBits;
	//copyColor &= redBits;
	//copyColor &= alphaBits;
	unsigned int tempColor = 0x00000000;
	tempColor |= alphaBits;
	tempColor |= redBits;
	tempColor |= greenBits;
	tempColor |= blueBits;

	return tempColor;//= (alphaBits | redBits | greenBits | blueBits);
}

//LERPS BETWEEN COLORS ONLY
unsigned int colorLerp(unsigned int lColor1, unsigned int lColor2, float cLratio)
{
	//BREAK UP THE VALUES FOR EACH COLOR ARGB AND THEN LERP THE CHANNELS INDIVIDUALLY
	unsigned int colorLerpCopy1 = lColor1;
	unsigned int colorLerpCopy2 = lColor2;
	unsigned int alphas1 = colorLerpCopy1 & 0xff000000;
	unsigned int reds1 = colorLerpCopy1 & 0x00FF0000;
	unsigned int greens1 = colorLerpCopy1 & 0x0000ff00;
	unsigned int blues1 = colorLerpCopy1 & 0x000000ff;
	unsigned int alphas2 = colorLerpCopy2 & 0xff000000;
	unsigned int reds2 = colorLerpCopy2 & 0x00FF0000;
	unsigned int greens2 = colorLerpCopy2 & 0x0000ff00;
	unsigned int blues2 = colorLerpCopy2 & 0x000000ff;

	alphas1 = alphas1 >> 24;
	alphas2 = alphas2 >> 24;
	unsigned int lerpAlphas = lerp(alphas1, alphas2, cLratio);
	lerpAlphas = lerpAlphas << 24;

	reds1 = reds1 >> 16;
	reds2 = reds2 >> 16;
	unsigned int lerpReds = lerp(reds1, reds2, cLratio);
	lerpReds = lerpReds << 16;

	greens1 = greens1 >> 8;
	greens2 = greens2 >> 8;
	unsigned int lerpGreens = lerp(greens1, greens2, cLratio);
	lerpGreens = lerpGreens << 8;

	unsigned int lerpBlues = lerp(blues1, blues2, cLratio);

	unsigned int finalLerpColor = 0x00000000;
	finalLerpColor |= lerpAlphas;
	finalLerpColor |= lerpReds;
	finalLerpColor |= lerpGreens;
	finalLerpColor |= lerpBlues;

	return finalLerpColor;
}

//CONVERTS NDC COORDINATES TO SCREEN COORDINATES
//vertex2D NDCtoScreenPixels(vertex3D NDCvert, float width, float height)
//{
//	//WIDTH AND HEIGHT ARE OF RASTER
//	//VERTEX 3D 
//	vertex2D  NDCtoRasterVert;
//
//	NDCtoRasterVert.x = (NDCvert.x + 1) * (width * 0.5);
//	NDCtoRasterVert.y = (1 - NDCvert.y) * (height * 0.5);
//	//X = (_VERT.X + 1) * (_WIDTH * 0.5);
//	//Y = (1 - _VERT.Y) * (_HEIGHT * 0.5);
//	
//	//VERT.Z = Z
//	//VERT.W = W
//	return NDCtoRasterVert;
//
//}


float implicitLineEquation(float impX, float impY, float impLineX1, float impLineX2, float impLineY1, float impLineY2)
{
	//float vecParallelToLineX = impLineX2 - impLineX1;
	//float vecParallelToLineY = impLineY2 - impLineY1;
	//
	//float gradientA = impLineY1 - impLineY2;
	//float gradientB = impLineX2 - impLineX1;

	float ans = ((impLineY1 - impLineY2) * impX + ((impLineX2 - impLineX1) * impY) + ((impLineX1 * impLineY2) - (impLineY1 * impLineX2)));
	return ans;



}

//COLORS IN A CERTAIN PIXEL IN A RASTER
unsigned int colorPixel(unsigned int pArr[], unsigned int x, int y, int widthOfRaster, int heightOfRaster, int color)
{
	if (x < widthOfRaster && y < heightOfRaster)
	{
		pArr[widthOfRaster * y + x] = color;
	}
	return 1;
}

//WILL DRAW A LINE BASED ON THE ALGORITHM, AND IN A CERTAIN COLOR
void bresenhamAlgorithm(unsigned int arr[], float x1, float x2, float y1, float y2, unsigned int color)
{
	//float x1 = 51.0f, y1 = 95.0f, x2 = 358.0f, y2 = 228.0f;
	float currY;
	float slope;
	float error = 0.0f;
	currY = y1;
	float currX = x1;

	float yDifference = y2 - y1;
	float xDifference = x2 - x1;
	slope = yDifference / xDifference;


	for (float i = x1; i < x2; i++)
	{
		colorPixel(arr, i, currY, 500, 500, color);
		error += slope;
		if (error > 0.5)
		{
			currY += 1, error -= 1;
		}
	}
	//	ColorCorrection(colorPixel(arr, x1, y1, 500, 500, 0xffffffff));
	//	ColorCorrection(colorPixel(arr, x2, y2, 500, 500, 0xffffffff));


}

//DRAWS A LINE BASED ON THE MIDPOINT
void midPointAlgorithm(unsigned int midArr[], float midX1, float midX2, float midY1, float midY2, unsigned int color)
{
	float midCurrY = midY1;
	float midCurrX = midX1;

	for (int i = midX1; i < midX2; i++)
	{
		ColorCorrection(colorPixel(midArr, i, midCurrY, 500, 500, color));
		float midpointX = i + 1;
		float midpointY = midCurrY + 0.5;
		//midCurrY += 1;
		if (implicitLineEquation(midpointX, midpointY, midX1, midX2, midY1, midY2) < 0)
			midCurrY += 1;
	}




}

//BASED ON PARAMETRIC
void parametricAlgorithm(unsigned int paraArr[], float paraX1, float paraX2, float paraY1, float paraY2, unsigned int color)
{
	float ratio;
	float currY = paraY1;
	unsigned int magenta;
	unsigned int cyan;
	magenta = 0x00ff00ff;
	cyan = 0x0000ffff;
	for (int i = paraX1; i < paraX2; i++)
	{
		ratio = (i - paraX1) / (paraX2 - paraX1);
		currY = lerp(paraY1, paraY2, ratio);
		float myLerp;
		//myLerp = lerp(0xffff00ff, 0xff00ffff, .6f);
		//color = myLerp;
		//myLerp = lerp(ColorCorrection(magenta), ColorCorrection(cyan), ratio);

		colorPixel(paraArr, i,  floor(currY + 0.5), 500, 500, colorLerp(cyan, magenta, ratio));


	}
}

//vertex2D vectorMatrixMultiply()
//{
//
//}














