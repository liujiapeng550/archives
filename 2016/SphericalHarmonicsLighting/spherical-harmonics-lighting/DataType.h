/**
	\file DataType.h
	\brief 数据类型定义
	\author 王延樑
*/
//------------------------------------------------------------------------------
#ifndef _AE_DATA_TYPE_H_
#define _AE_DATA_TYPE_H_ "DataType.h"
//------------------------------------------------------------------------------
#include <cstddef>
#include "Defines.h"
//------------------------------------------------------------------------------
/**
	\var aeVOID
	\brief 空类型
*/
#ifdef _MSC_VER
typedef void aeVOID;
#else
#define aeVOID void
#endif//_MSC_VER
/**
	\var aePVOID;
	\brief 空类型指针
*/
typedef void* aePVOID;
/**
	\var aeQWORD
	\brief 四字类型
*/
typedef unsigned long long aeQWORD;
#if defined(_MSC_VER)
/**
	\var aeDWORD
	\brief 双字类型
*/
typedef unsigned long aeDWORD;
#elif defined(__GNUC__)
#	if defined(__i386__) || defined(__i686__) || defined(__arm__)
/**
	\var aeDWORD
	\brief 双字类型
*/
typedef unsigned long aeDWORD;
#	elif defined(__x86_64__) || defined(__arm64__) || defined(__aarch64__)
/**
	\var aeDWORD
	\brief 双字类型
*/
typedef unsigned int aeDWORD;
#	else
#		error Not supported platform.
#	endif
#else
#	error Not supported compiler.
#endif
/**
	\var aeWORD
	\brief 字类型
*/
typedef unsigned short aeWORD;
/**
	\var aeBYTE
	\brief 字节类型
*/
typedef unsigned char aeBYTE;
/**
	\var aeINT8
	\brief 8位整数类型
*/
typedef signed char aeINT8;
/**
	\var aeINT16
	\brief 16位整数类型
*/
typedef signed short aeINT16;
#if defined(_MSC_VER)
/**
	\var aeINT32
	\brief 32位整数类型
*/
typedef signed long aeINT32;
#elif defined(__GNUC__)
#	if defined(__i386__) || defined(__i686__) ||  defined(__arm__)
/**
	\var aeINT32
	\brief 32位整数类型
*/
typedef signed long aeINT32;
#	elif defined(__x86_64__) || defined(__arm64__) || defined(__aarch64__)
/**
	\var aeINT32
	\brief 32位整数类型
*/
typedef signed int aeINT32;
#	else
#		error Not supported platform.
#	endif
#else
#	error Not supported compiler.
#endif
/**
	\var aeINT64
	\brief 64位整数类型
*/
typedef signed long long aeINT64;
/**
	\var aeUINT8
	\brief 8位无符号整数类型
*/
typedef unsigned char aeUINT8;
/**
	\var aeUINT16
	\brief 16位无符号整数类型
*/
typedef unsigned short aeUINT16;
#if defined(_MSC_VER)
/**
	\var aeUINT32
	\brief 32位无符号整数类型
*/
typedef unsigned long aeUINT32;
#elif defined(__GNUC__)
#	if defined(__i386__) || defined(__i686__) || defined(__arm__)
/**
	\var aeUINT32
	\brief 32位无符号整数类型
*/
typedef unsigned long aeUINT32;
#	elif defined(__x86_64__) || defined(__arm64__) || defined(__aarch64__)
/**
	\var aeUINT32
	\brief 32位无符号整数类型
*/
typedef unsigned int aeUINT32;
#	else
#		error Not supported platform.
#	endif
#else
#	error Not supported compiler.
#endif
/**
	\var aeUINT64
	\brief 64位无符号整数类型
*/
typedef unsigned long long aeUINT64;
/**
	\var aeLONG
	\brief 长整数类型
*/
typedef signed long aeLONG;
/**
	\var aeINT
	\brief 整数类型
*/
typedef signed int aeINT;
/**
	\var aeSHORT
	\brief 短整数类型
*/
typedef signed short aeSHORT;
/**
	\var aeCHAR
	\brief 字符类型
*/
typedef char aeCHAR;
/**
	\var aeWCHAR
	\brief 宽字符类型
*/
typedef wchar_t aeWCHAR;
/**
	\var aeULONG
	\brief 无符号长整数类型
*/
typedef unsigned long aeULONG;
/**
	\var aeUINT
	\brief 无符号整数类型
*/
typedef unsigned long aeUINT;
/**
	\var aeUSHORT
	\brief 无符号短整数类型
*/
typedef unsigned short aeUSHORT;
/**
	\var aeUCHAR
	\brief 无符号字符类型
*/
typedef unsigned char aeUCHAR;
/**
	\var aeEXTENDED
	\brief 128位浮点类型
 */
typedef long double aeEXTENDED;
/**
	\var aeDOUBLE
	\brief 双精度浮点类型
*/
typedef double aeDOUBLE;
/**
	\var aeFLOAT
	\brief 单精度浮点类型
*/
typedef float aeFLOAT;
/**
	\var aeFLOAT64
	\brief 64位浮点类型
*/
typedef double aeFLOAT64;
/**
	\var aeFLOAT32
	\brief 32位浮点类型
*/
typedef float aeFLOAT32;
/**
	\var aeBOOL
	\brief 逻辑类型
*/
typedef bool aeBOOL;
/**
	\var aeHANDLE
	\brief 句柄类型
*/
typedef void* aeHANDLE;
/**
	\var aePCHAR
	\brief 字符指针类型
*/
typedef char* aePCHAR;
/**
	\var aeSIZE
	\brief 长度类型
*/
typedef size_t aeSIZE;
/**
	\var aeRESULT
	\brief 返回值类型
*/
typedef unsigned long aeRESULT;
//------------------------------------------------------------------------------
/**
	\brief 2d点
*/
struct aePoint2
{
	aeFLOAT X;
	aeFLOAT Y;
};
//------------------------------------------------------------------------------
/**
	\brief 轴对齐的矩形
*/
struct aeRect
{
	union
	{
		union
		{
			aePoint2 LeftTop;
			struct
			{
				aeFLOAT X1;
				aeFLOAT Y1;
			};
		};
		union
		{
			aePoint2 RightBottom;
			struct
			{
				aeFLOAT X2;
				aeFLOAT Y2;
			};
		};
		aeFLOAT D[4];
	};
};
//------------------------------------------------------------------------------
/**
	\brief 整数坐标2D点
*/
struct aePoint2i
{
	/**
		\brief 横坐标
	*/
	aeINT X;
	/**
		\brief 纵坐标
	*/
	aeINT Y;
};
inline aePoint2i Point2i(aeINT x, aeINT y)
{
	aePoint2i ret = {x, y};
	return ret;
}
//------------------------------------------------------------------------------
inline aePoint2i operator-(const aePoint2i& l, const aePoint2i& r)
{
	aePoint2i ret = {l.X - r.X, l.Y - r.Y};
	return ret;
}
//------------------------------------------------------------------------------
inline aeBOOL operator==(const aePoint2i& l, const aePoint2i& r)
{
	return l.X == r.X && l.Y == r.Y;
}
//------------------------------------------------------------------------------
inline aeBOOL operator!=(const aePoint2i& l, const aePoint2i& r)
{
	return l.X != r.X || l.Y != r.Y;
}
//------------------------------------------------------------------------------
/**
	\brief 整数坐标3D点
*/
struct aePoint3i
{
	/**
		\brief 横坐标
	*/
	aeINT X;
	/**
		\brief 纵坐标
	*/
	aeINT Y;
	/**
		\brief 深度坐标
	*/
	aeINT Z;
};
//------------------------------------------------------------------------------
/**
	\brief 整数坐标矩形
*/
struct aeRecti
{
	aeRecti(aeINT left, aeINT top, aeINT right, aeINT bottom)
		: Left(left), Top(top), Right(right), Bottom(bottom)
	{

	}
	union
	{
		struct  
		{
			/**
				\brief 左上角坐标
			*/
			union
			{
				aePoint2i LeftTop;
				struct  
				{
					/**
						\brief 左侧坐标
					*/
					aeINT Left;
					/**
						\brief 上坐标
					*/
					aeINT Top;
				};
			};
			/**
				\brief 右下角坐标
			*/
			union
			{
				aePoint2i RightBottom;
				struct  
				{
					/**
						\brief 右侧坐标
					*/
					aeINT Right;
					/**
						\brief 下坐标
					*/
					aeINT Bottom;
				};
			};
		};
		aeINT D[4];
	};
};
//------------------------------------------------------------------------------
inline aeBOOL In(const aeRecti* r, const aePoint2i* p)
{
	return r->Left <= p->X && p->X <= r->Right
		&& r->Top <= p->Y && p->Y <= r->Bottom; 
}
//------------------------------------------------------------------------------
/**
	\brief 信息层级
*/
enum aeMessageLevel
{
	mlDebug   = 0x00,
	mlWarning = 0x40,
	mlError   = 0x80,
};
//------------------------------------------------------------------------------
//定义基本数据
/**
	\brief 无效句柄标志
*/
const aeHANDLE c_InvalidHandle = (aeHANDLE)0;
//------------------------------------------------------------------------------
#endif//_AE_DATA_TYPE_H_
