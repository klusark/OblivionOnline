#include "Commands_Math.h"
#include "GameAPI.h"
#include "ParamInfos.h"
#include "Utilities.h"

//Some basic maths functions

#if OBLIVION

// basic math functions

bool Cmd_SquareRoot_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = sqrt(arg);

	return true;
}

bool Cmd_Log_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = log(arg);

	return true;
}
bool Cmd_Exp_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = exp(arg);

	return true;
}

bool Cmd_Log10_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = log10(arg);

	return true;
}

bool Cmd_Pow_Execute(COMMAND_ARGS)
{
	*result = 0;

	float f1 = 0;
	float f2 = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &f1, &f2)) return true;

	*result = pow(f1,f2);

	return true;
}

bool Cmd_Floor_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = floor(arg);

	return true;
}
bool Cmd_Ceil_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = ceil(arg);

	return true;
}
bool Cmd_Abs_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = abs(arg);

	return true;
}
bool Cmd_Rand_Initialized=false;
bool Cmd_Rand_Execute(COMMAND_ARGS)
{
	if(!Cmd_Rand_Initialized) {
		Cmd_Rand_Initialized=true;
		MersenneTwister::init_genrand(GetTickCount());
	}
	*result = 0;

	float rangeMin = 0;
	float rangeMax = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &rangeMin, &rangeMax)) return true;

	if(rangeMax < rangeMin)
	{
		float	temp = rangeMin;
		rangeMin = rangeMax;
		rangeMax = temp;
	}

	float	range = rangeMax - rangeMin;

	double	value = MersenneTwister::genrand_real2() * range;
	value += rangeMin;

	*result = value;

	return true;
}

// trig functions using radians

bool Cmd_Sin_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = sin(arg);

	return true;
}
bool Cmd_Cos_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = cos(arg);

	return true;
}
bool Cmd_Tan_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = tan(arg);

	return true;
}
bool Cmd_ASin_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = asin(arg);

	return true;
}
bool Cmd_ACos_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = acos(arg);

	return true;
}
bool Cmd_ATan_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = atan(arg);

	return true;
}

bool Cmd_ATan2_Execute(COMMAND_ARGS)
{
	*result = 0;

	float f1 = 0;
	float f2 = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &f1, &f2)) return true;

	*result = atan2(f1,f2);

	return true;
}
bool Cmd_Sinh_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = sinh(arg);

	return true;
}
bool Cmd_Cosh_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = cosh(arg);

	return true;
}
bool Cmd_Tanh_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = tanh(arg);

	return true;
}

// trig functions using degrees
#define DEGTORAD 0.01745329252f

bool Cmd_dSin_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = sin(arg*DEGTORAD);

	return true;
}
bool Cmd_dCos_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = cos(arg*DEGTORAD);

	return true;
}
bool Cmd_dTan_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = tan(arg*DEGTORAD);

	return true;
}
bool Cmd_dASin_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = asin(arg)/DEGTORAD;

	return true;
}
bool Cmd_dACos_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = acos(arg)/DEGTORAD;

	return true;
}
bool Cmd_dATan_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = atan(arg)/DEGTORAD;

	return true;
}
bool Cmd_dATan2_Execute(COMMAND_ARGS)
{
	*result = 0;

	float f1 = 0;
	float f2 = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &f1, &f2)) return true;

	*result = atan2(f1,f2)/DEGTORAD;

	return true;
}
bool Cmd_dSinh_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = sinh(arg*DEGTORAD);

	return true;
}
bool Cmd_dCosh_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = cosh(arg*DEGTORAD);

	return true;
}
bool Cmd_dTanh_Execute(COMMAND_ARGS)
{
	*result = 0;

	float arg = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
		return true;

	*result = tanh(arg*DEGTORAD);

	return true;
}

bool Cmd_LeftShift_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32	lhs = 0;
	UInt32	rhs = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &lhs, &rhs))
		return true;

	if(rhs >= 32)
		*result = 0;
	else
		*result = lhs << rhs;

	return true;
}

bool Cmd_RightShift_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32	lhs = 0;
	UInt32	rhs = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &lhs, &rhs))
		return true;

	if(rhs >= 32)
		*result = 0;
	else
		*result = lhs >> rhs;

	return true;
}

bool Cmd_LogicalAnd_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32	lhs = 0;
	UInt32	rhs = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &lhs, &rhs))
		return true;

	*result = lhs & rhs;

	return true;
}

bool Cmd_LogicalOr_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32	lhs = 0;
	UInt32	rhs = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &lhs, &rhs))
		return true;

	*result = lhs | rhs;

	return true;
}

bool Cmd_LogicalXor_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32	lhs = 0;
	UInt32	rhs = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &lhs, &rhs))
		return true;

	*result = lhs ^ rhs;

	return true;
}

#pragma warning (push)
#pragma warning (disable : 4319)

bool Cmd_LogicalNot_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32	lhs = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &lhs))
		return true;

	*result = (double)(~lhs);

	return true;
}

#pragma warning (pop)

bool Cmd_Fmod_Execute(COMMAND_ARGS)
{
	*result = 0;
	float x = 0.0;
	float n = 0.0;
	float offset = 0.0;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &x, &n, &offset);

	float answer = x - n * floor(x/n);
	if (offset != 0) {
		float bump = n * floor(offset/n);
		float bound = offset - bump;
		answer += bump;
		bool bBigger = (n > 0);
		if ( (bBigger && answer < bound) || (!bBigger && answer > bound) ) {
			answer += n;
		}
	}

	return true;
}

#endif

CommandInfo kCommandInfo_SquareRoot =
{
	"SquareRoot",
	"sqrt",
	0,
	"Calculates the root of a value",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_SquareRoot_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Pow =
{
	"Pow",
	"pow",
	0,
	"Calculates one value raised to the power of another",
	0,
	2,
	kParams_TwoFloats,
	HANDLER(Cmd_Pow_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Log =
{
	"Log",
	"log",
	0,
	"Calculates the natural log of a value",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Log_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Exp =
{
	"Exp",
	"exp",
	0,
	"Calculates the exponential of a value",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Exp_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Log10 =
{
	"Log10",
	"log10",
	0,
	"Calculates the base 10 log of a value",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Log10_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Floor =
{
	"Floor",
	"flr",
	0,
	"Returns the nearest whole number that's less than a float",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Floor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Ceil =
{
	"Ceil",
	"ceil",
	0,
	"Returns the nearest whole number that's greater than a float",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Ceil_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Abs =
{
	"Abs",
	"abs",
	0,
	"Returns the absolute value of a float",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Abs_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Rand =
{
	"Rand",
	"r",
	0,
	"Returns a float between min and max",
	0,
	2,
	kParams_TwoFloats,
	HANDLER(Cmd_Rand_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_Sin =
{
	"rSin",
	"rsin",
	0,
	"Calculates the sin of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Sin_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Cos =
{
	"rCos",
	"rcos",
	0,
	"Calculates the cos of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Cos_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Tan =
{
	"rTan",
	"rtan",
	0,
	"Calculates the tan of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Tan_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
CommandInfo kCommandInfo_ASin =
{
	"rASin",
	"rasin",
	0,
	"Calculates the arcsin of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_ASin_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ACos =
{
	"rACos",
	"racos",
	0,
	"Calculates the arccos of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_ACos_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ATan =
{
	"rATan",
	"ratan",
	0,
	"Calculates the arctan of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_ATan_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ATan2 =
{
	"rATan2",
	"ratan2",
	0,
	"Calculates the arctan of a value in radians",
	0,
	2,
	kParams_TwoFloats,
	HANDLER(Cmd_ATan2_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Sinh =
{
	"rSinh",
	"rsinh",
	0,
	"Calculates the hyperbolic sin of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Sinh_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Cosh =
{
	"rCosh",
	"rcosh",
	0,
	"Calculates the hyperbolic cos of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Cosh_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_Tanh =
{
	"rTanh",
	"rtanh",
	0,
	"Calculates the hyperbolic tan of a value in radians",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_Tanh_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
CommandInfo kCommandInfo_dSin =
{
	"Sin",
	"dsin",
	0,
	"Calculates the sin of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dSin_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dCos =
{
	"Cos",
	"dcos",
	0,
	"Calculates the cos of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dCos_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dTan =
{
	"Tan",
	"dtan",
	0,
	"Calculates the tan of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dTan_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
CommandInfo kCommandInfo_dASin =
{
	"ASin",
	"dasin",
	0,
	"Calculates the arcsin of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dASin_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dACos =
{
	"ACos",
	"dacos",
	0,
	"Calculates the arccos of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dACos_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dATan =
{
	"ATan",
	"datan",
	0,
	"Calculates the arctan of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dATan_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
CommandInfo kCommandInfo_dATan2 =
{
	"ATan2",
	"datan2",
	0,
	"Calculates the arctan of a value in degrees",
	0,
	2,
	kParams_TwoFloats,
	HANDLER(Cmd_dATan2_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dSinh =
{
	"Sinh",
	"dsinh",
	0,
	"Calculates the hyperbolic sin of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dSinh_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dCosh =
{
	"Cosh",
	"dcosh",
	0,
	"Calculates the hyperbolic cos of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dCosh_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_dTanh =
{
	"Tanh",
	"dtanh",
	0,
	"Calculates the hyperbolic tan of a value in degrees",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_dTanh_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_LeftShift =
{
	"LeftShift",
	"",
	0,
	"Shifts a 32-bit integer left the specified number of bits",
	0,
	2,
	kParams_TwoInts,
	HANDLER(Cmd_LeftShift_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_RightShift =
{
	"RightShift",
	"",
	0,
	"Shifts an unsigned 32-bit integer right the specified number of bits",
	0,
	2,
	kParams_TwoInts,
	HANDLER(Cmd_RightShift_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_LogicalAnd =
{
	"LogicalAnd",
	"",
	0,
	"Performs a logical AND between two 32-bit integers.",
	0,
	2,
	kParams_TwoInts,
	HANDLER(Cmd_LogicalAnd_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_LogicalOr =
{
	"LogicalOr",
	"",
	0,
	"Performs a logical OR between two 32-bit integers.",
	0,
	2,
	kParams_TwoInts,
	HANDLER(Cmd_LogicalOr_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_LogicalXor =
{
	"LogicalXor",
	"",
	0,
	"Performs a logical XOR between two 32-bit integers.",
	0,
	2,
	kParams_TwoInts,
	HANDLER(Cmd_LogicalXor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_LogicalNot =
{
	"LogicalNot",
	"",
	0,
	"Performs a logical NOT on a 32-bit integer.",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_LogicalNot_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_Fmod[3] =
{
	{	"x",	kParamType_Float,	0 },
	{	"n",	kParamType_Float,	0 },
	{	"offset",	kParamType_Float,	1 },
};

CommandInfo kCommandInfo_Fmod =
{
	"fmod",
	"",
	0,
	"returns the result of a floating point modulous of the passed floats",
	0,
	3,
	kParams_Fmod,
	HANDLER(Cmd_Fmod_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
