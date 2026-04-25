#pragma once

namespace Umi
{
	struct Rect
	{
		float tx; // Texture X coordinate
		float ty; // Texture Y coordinate
		float tw; // Texture Width
		float th; // Texture Height

		Rect(float tx = 0.0f, float ty = 0.0f, float tw = 1.0f, float th = 1.0f) : tx(tx), ty(ty), tw(tw), th(th) {};

		Rect operator= (const Rect& other)
		{
			tx = other.tx;
			ty = other.ty;
			tw = other.tw;
			th = other.th;
			return *this;
		}

		Rect operator+ (const Rect& other)
		{
			return Rect(tx + other.tx, ty + other.ty, tw + other.tw, th + other.th);
		}

		Rect operator- (const Rect& other)
		{
			return Rect(tx - other.tx, ty - other.ty, tw - other.tw, th - other.th);
		}
	};
}