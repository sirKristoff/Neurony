#include <math.h>

#include "funs.h"


const Input kBeta= 1.0;

Input ident( Input x )
{
	return (x);
}

Input d_ident( Input )
{
	return Input(1.0);
}


Input uni_sigm( Input x )
{
	return 1.0/(Input(1.0)+exp(-kBeta*x));
}

Input d_uni_sigm( Input x )
{
	const Input sigm= uni_sigm(x);
	return kBeta * sigm * (1.0 - sigm);
}

Input bi_sigm( Input x )
{
	return 2.0/(Input(1.0)+exp(-kBeta*x))-1.0;
//	return (1-exp(-kBeta*x))/(1+exp(-kBeta*x));
}

Input d_bi_sigm( Input x )
{
	const Input sigm= bi_sigm(x);
	return 0.5*kBeta * (1.0 - sigm*sigm);
}
