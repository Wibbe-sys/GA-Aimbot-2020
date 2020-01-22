#include "Includes.h"
#include <intrin.h>

D3DXVECTOR3 CalcAngle(register const D3DXVECTOR3& src, register const D3DXVECTOR3& dst)
{
	
	const auto sqrtss = [](float in) {
		__m128 reg = _mm_load_ss(&in);
		return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
	};


	D3DXVECTOR3 angles;

	//f� delta mellan k�ll och destinationsvektorer
	D3DXVECTOR3 delta = src - dst;

	//hitta hypotenusan med hj�lp av pythagorassats
	//d� f�r vi vector till fienden
	float hyp = sqrtss(delta.x * delta.x + delta.y * delta.y);

	//nu m�ste vi hitta den vinkel som beh�vs f�r att sikta mot vektorn (aim angles)
	angles.x = asinf(delta.z / hyp)		* (180.0f / M_PI);
	angles.y = atanf(delta.y / delta.x)	* (180.0f / M_PI) + !((*(DWORD*)&delta.x) >> 31 & 1) * 180.0f;


	angles.z = 0.0f;

	return angles;
}
