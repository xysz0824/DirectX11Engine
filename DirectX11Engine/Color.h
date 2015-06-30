#ifndef _COLOR_H
#define _COLOR_H

struct Color
{
	float r, g, b, a;
	const float* ToArray()
	{
		return (const float*)this;
	}

	static Color Black() { return{ 0, 0, 0, 1 }; }
	static Color Red() { return{ 1, 0, 0, 1 }; }
	static Color Green() { return{ 0, 1, 0, 1 }; }
	static Color Blue() { return{ 0, 0, 1, 1 }; }
};
#endif