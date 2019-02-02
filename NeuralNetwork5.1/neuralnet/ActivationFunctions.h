#pragma once

#include <cmath>
namespace dp {
	
	namespace {
		typedef double(*fptr)(double);

		static double fncRELU(double n) { return (n >= 0) ? n : 0.0; }
		static double fncLinear(double n) { return n; }
		static double fncSigLog(double n) { return 1.0 / (1.0 + exp(-n)); }
		static double fncHyperTan(double n) { return tanh(n); }

		static double derivateFncLinear(double n) { return 1.0; }
		static double derivateFncSigLog(double n) { return n * (1.0 - n); }
		static double derivativeFncHyperTan(double n) { return 1.0 - (n*n); }
		static double derivativeFncRELU(double n) { return (n >= 0) ? 1.0 : 0.0; }
	}
	enum ActivationFncENUM { LINEAR = 0, SIGLOG = 1, HYPERTAN = 2, RELU = 3, };
	enum GradientDescent { STOCHASTIC=0,MINI_BATCH=1,BATCH=2};

	fptr activationFnc[4] = { fncLinear,fncSigLog,fncHyperTan, fncRELU };
	fptr derivateActivationFnc[4] = { derivateFncLinear,derivateFncSigLog,derivativeFncHyperTan,derivativeFncRELU };
}
