#include "Commands_Math.h"
#include "GameAPI.h"
#include "ParamInfos.h"

//Some basic maths functions

#if OBLIVION

namespace MersenneTwister
{

/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

#undef N
#undef M
#undef MATRIX_A
#undef UPPER_MASK
#undef LOWER_MASK

};

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
